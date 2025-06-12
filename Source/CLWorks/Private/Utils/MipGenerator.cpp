#include "MipGenerator.h"

#include <algorithm>

namespace MipGenerator
{
	template<typename V, typename C>
	void GenerateMip(std::vector<Mip>& output,
				     V* const src,
				     size_t srcWidth, 
				     size_t srcHeight,
				     size_t srcLayers,
				     uint8_t srcChannels)
	{
		output.clear();

		size_t currentWidth = srcWidth;
		size_t currentHeight = srcHeight;
		V* currentData = src;

		// Initial Mip
		output.push_back(Mip{ currentWidth,
						      currentHeight,
						      srcLayers,
						      srcChannels,
						      src });

		while (currentWidth > 1 || currentHeight > 1)
		{
			size_t nextWidth	= std::max(static_cast<size_t>(1), static_cast<size_t>(std::roundf(currentWidth * 0.5f)));
			size_t nextHeight	= std::max(static_cast<size_t>(1), static_cast<size_t>(std::roundf(currentHeight * 0.5f)));
			V* nextData = new V[nextWidth * nextHeight * srcChannels * srcLayers];

			for (size_t layer = 0; layer < srcLayers; ++layer)
			{
				size_t currentLayerOffset = layer * currentWidth * currentHeight * srcChannels;

				for (size_t y = 0; y < nextHeight; ++y)
				{
					for (size_t x = 0; x < nextWidth; ++x)
					{
						std::vector<C> channelPixel(srcChannels, 0);

						float count = 0;
						for (size_t dy = 0; dy < 2; ++dy)
						{
							for (size_t dx = 0; dx < 2; ++dx)
							{
								size_t srcX = std::min(currentWidth - 1, x * 2 + dx);
								size_t srcY = std::min(currentHeight - 1, y * 2 + dy);
								size_t srcIndex = ((srcY * currentWidth + srcX) * srcChannels) + currentLayerOffset;

								for (int c = 0; c < srcChannels; ++c)
									channelPixel[c] += currentData[srcIndex + c];

								++count;
							}
						}

						size_t nextLayerOffset = layer * nextWidth * nextHeight * srcChannels;
						const size_t dstIndex = ((y * nextWidth + x) * srcChannels) + nextLayerOffset;
						for (int c = 0; c < srcChannels; ++c)
							nextData[dstIndex + c] = static_cast<V>(std::floor(channelPixel[c] / count));
					}
				}
			}

			output.push_back({ nextWidth, 
							   nextHeight, 
							   srcLayers, 
							   srcChannels, 
							   nextData });

			currentData = std::move(nextData);
			currentWidth = nextWidth;
			currentHeight = nextHeight;
		}
	}

	void GenerateMipsInt8(std::vector<Mip>& output,
						  uint8_t* const src,
						  size_t srcWidth, 
						  size_t srcHeight,
						  size_t srcLayers,
						  uint8_t srcChannels)
	{
		GenerateMip<uint8_t, size_t>(output, src, srcWidth, srcHeight, srcLayers, srcChannels);
	}

	void GenerateMipsUInt32(std::vector<Mip>& output, 
							uint32_t* const src, 
							size_t srcWidth, 
							size_t srcHeight, 
							size_t srcLayers, 
							uint8_t srcChannels)
	{
		GenerateMip<uint32_t, size_t>(output, src, srcWidth, srcHeight, srcLayers, srcChannels);
	}

	void GenerateMipsInt32(std::vector<Mip>& output, 
						   int32_t* const src, 
						   size_t srcWidth, 
						   size_t srcHeight, 
						   size_t srcLayers, 
						   uint8_t srcChannels)
	{
		GenerateMip<int32_t, size_t>(output, src, srcWidth, srcHeight, srcLayers, srcChannels);
	}

	void GenerateMipsFloat16(std::vector<Mip>& output,
							 FFloat16* const src,
						     size_t srcWidth, 
						     size_t srcHeight, 
							 size_t srcLayers,
						     uint8_t srcChannels)
	{
		GenerateMip<FFloat16, float>(output, src, srcWidth, srcHeight, srcLayers, srcChannels);
	}

	void GenerateMipsFloat(std::vector<Mip>& output,
						   float* const src,
						   size_t srcWidth, 
						   size_t srcHeight, 
						   size_t srcLayers,
						   uint8_t srcChannels)
	{
		GenerateMip<float, float>(output, src, srcWidth, srcHeight, srcLayers, srcChannels);
	}
}