#pragma once

#include "Core/ImageDefines.h"

#include <vector>

namespace MipGenerator
{
	static void GenerateMipsInt8(std::vector<Mip>& output,
								 uint8_t* const src,
								 size_t srcWidth, 
								 size_t srcHeight,
								 size_t srcLayers,
								 uint8_t srcChannels);

	static void GenerateMipsFloat16(std::vector<Mip>& output,
									FFloat16* const src,
									size_t srcWidth,
									size_t srcHeight,
									size_t srcLayers,
									uint8_t srcChannels);

	static void GenerateMipsFloat(std::vector<Mip>& output,
								  float* const src,
								  size_t srcWidth,
								  size_t srcHeight,
								  size_t srcLayers,
								  uint8_t srcChannels);
}