#include "Render/UTextureUtils.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

#include "Render/BlitTextureShaders.h"

namespace UTextureUtils
{
	bool BlitTextureToRenderTarget(const TObjectPtr<UTexture2D> source, 
								   TObjectPtr<UTextureRenderTarget2D>& output, 
								   FIntPoint subRect)
	{
		if (!source || !output)
			return false;
		
		ENQUEUE_RENDER_COMMAND(CopyTextureCommand)([source, output, subRect](FRHICommandListImmediate& RHICmdList)
		{
			FRDGBuilder GraphBuilder(RHICmdList);

			TRefCountPtr<IPooledRenderTarget> sourceRT = CreateRenderTarget(source->GetResource()->TextureRHI, TEXT("Source"));
			FRDGTextureRef source_rdg = GraphBuilder.RegisterExternalTexture(sourceRT);

			TRefCountPtr<IPooledRenderTarget> outputRT = CreateRenderTarget(output->GetRenderTargetResource()->GetRenderTargetTexture(), TEXT("Output"));
			FRDGTextureRef output_rdg = GraphBuilder.RegisterExternalTexture(outputRT);


			FBlitTextureShadersCS::FParameters* parameters = GraphBuilder.AllocParameters<FBlitTextureShadersCS::FParameters>();
			parameters->Input = source_rdg;
			parameters->Sampler = TStaticSamplerState<SF_Point, AM_Wrap, AM_Wrap>::GetRHI();
			parameters->Output = GraphBuilder.CreateUAV(output_rdg);

			FBlitTextureShadersCS::FPermutationDomain Permutation;
			Permutation.Set<FBlitTextureShadersCS::FChannelFormatPermutation>(FBlitTextureShadersCS::EChannelFormat::Float4);

			const FGlobalShaderMap* globalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
			auto blitShader = globalShaderMap->GetShader<FBlitTextureShadersCS>(Permutation);


			FIntPoint Size = subRect != FIntPoint::ZeroValue ? subRect : output->GetRenderTargetResource()->GetSizeXY();

			FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(Size, FIntPoint(8, 8));

			FComputeShaderUtils::AddPass(GraphBuilder,
										 RDG_EVENT_NAME("BlitTextureToRT_CS"),
										 blitShader,
										 parameters,
										 GroupCount);

			GraphBuilder.Execute();
		});

		return true;
	}
};