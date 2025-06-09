#include "Render/UTextureUtils.h"

#include "RenderGraphBuilder.h"

namespace UTextureUtils
{
	bool CopyTextureToRenderTarget(const TObjectPtr<UTexture2D> source, 
								   TObjectPtr<UTextureRenderTarget2D>& output, 
								   FIntPoint subRect)
	{
		if (!source || !output)
			return false;
		
		ENQUEUE_RENDER_COMMAND(CopyTextureCommand)([source, output, subRect](FRHICommandListImmediate& RHICmdList)
		{
			FRDGBuilder GraphBuilder(RHICmdList);

			FRDGTextureRef RDGInput = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(source->GetResource()->TextureRHI, TEXT("SourceTexture")));
			FRDGTextureRef RDGOutput = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(output->GetRenderTargetResource()->GetRenderTargetTexture(), TEXT("TargetRT")));


			GraphBuilder.Execute();
		});

		return true;
	}
};