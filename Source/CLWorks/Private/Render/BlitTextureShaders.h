#pragma once

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "ShaderParameterMacros.h"

class FBlitTextureShadersCS : public FGlobalShader
{
public:
	enum class EChannelFormat : uint8
	{
		Float,
		Float4,
		Int,
		Int4,

		MAX
	};
public:
	DECLARE_GLOBAL_SHADER(FBlitTextureShadersCS);
	
	SHADER_USE_PARAMETER_STRUCT(FBlitTextureShadersCS, FGlobalShader);

	class FChannelFormatPermutation : SHADER_PERMUTATION_ENUM_CLASS("CHANNEL_FORMAT", EChannelFormat);
	using FPermutationDomain = TShaderPermutationDomain<FChannelFormatPermutation>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FIntPoint, Resolution)
		SHADER_PARAMETER_SAMPLER(SamplerState, Sampler)

		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, Input)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, Output)
	END_SHADER_PARAMETER_STRUCT()
public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};