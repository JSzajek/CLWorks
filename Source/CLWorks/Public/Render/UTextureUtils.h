#pragma once

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"

namespace UTextureUtils
{
	static bool BlitTextureToRenderTarget(const TObjectPtr<UTexture2D> source,
										  TObjectPtr<UTextureRenderTarget2D>& output,
										  FIntPoint subRect = FIntPoint::ZeroValue);
};