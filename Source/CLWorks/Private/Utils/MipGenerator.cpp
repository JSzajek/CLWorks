#include "MipGenerator.h"

#include <algorithm>

namespace MipGenerator
{
	void GenerateMipsInt8(std::vector<Mip>& output,
						  uint8_t* const src,
						  size_t srcWidth, 
						  size_t srcHeight,
						  size_t srcLayers,
						  uint8_t srcChannels)
	{
		output.clear();

		size_t currentWidth = srcWidth;
		size_t currentHeight = srcHeight;
		uint8_t* currentData = src;

		// Initial Mip
		output.push_back(Mip{ currentWidth,
							   currentHeight,
							   srcLayers,
							   srcChannels,
							   src });

		while (currentWidth > 1 || currentHeight > 1)
		{
			size_t nextWidth = std::max((size_t)1, currentWidth / 2);
			size_t nextHeight = std::max((size_t)1, currentHeight / 2);
			uint8_t* nextData = new uint8_t[nextWidth * nextHeight * srcChannels * srcLayers];

			for (size_t layer = 0; layer < srcLayers; ++layer)
			{
				size_t currentLayerOffset = layer * currentWidth * currentHeight * srcChannels;

				for (size_t y = 0; y < nextHeight; ++y)
				{
					for (size_t x = 0; x < nextWidth; ++x)
					{
						std::vector<size_t> channelPixel(srcChannels, 0);

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
							nextData[dstIndex + c] = static_cast<uint8_t>(std::floor(channelPixel[c] / count));
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

	void GenerateMipsFloat16(std::vector<Mip>& output,
							 FFloat16* const src,
						     size_t srcWidth, 
						     size_t srcHeight, 
							 size_t srcLayers,
						     uint8_t srcChannels)
	{
#if 0
		output.clear();

		size_t currentWidth = srcWidth;
		size_t currentHeight = srcHeight;
		FFloat16* currentData = src;

		// Initial Mip
		output.push_back({ currentWidth, currentHeight, srcChannels, src });

		while (currentWidth > 1 || currentHeight > 1)
		{
			size_t nextWidth = std::max((size_t)1, currentWidth / 2);
			size_t nextHeight = std::max((size_t)1, currentHeight / 2);
			FFloat16* nextData = new FFloat16[nextWidth * nextHeight * srcChannels];

			for (size_t y = 0; y < nextHeight; ++y)
			{
				for (size_t x = 0; x < nextWidth; ++x)
				{
					std::vector<float> channelPixel(srcChannels, 0);

					float count = 0;
					for (size_t dy = 0; dy < 2; ++dy)
					{
						for (size_t dx = 0; dx < 2; ++dx)
						{
							size_t srcX = std::min(currentWidth - 1, x * 2 + dx);
							size_t srcY = std::min(currentHeight - 1, y * 2 + dy);
							size_t srcIndex = (srcY * currentWidth + srcX) * srcChannels;

							for (int c = 0; c < srcChannels; ++c)
								channelPixel[c] += currentData[srcIndex + c];

							++count;
						}
					}

					const size_t dstIndex = (y * nextWidth + x) * srcChannels;
					for (int c = 0; c < srcChannels; ++c)
						nextData[dstIndex + c] = static_cast<FFloat16>(channelPixel[c] / count);
				}
			}

			output.push_back({ nextWidth, nextHeight, srcChannels, nextData });

			currentData = std::move(nextData);
			currentWidth = nextWidth;
			currentHeight = nextHeight;
		}
#endif
	}

	void GenerateMipsFloat(std::vector<Mip>& output,
						   float* const src,
						   size_t srcWidth, 
						   size_t srcHeight, 
						   size_t srcLayers,
						   uint8_t srcChannels)
	{
#if 0
		output.clear();

		size_t currentWidth = srcWidth;
		size_t currentHeight = srcHeight;
		float* currentData = src;

		// Initial Mip
		output.push_back({ currentWidth, currentHeight, srcChannels, src });

		while (currentWidth > 1 || currentHeight > 1)
		{
			size_t nextWidth = std::max((size_t)1, currentWidth / 2);
			size_t nextHeight = std::max((size_t)1, currentHeight / 2);
			float* nextData = new float[nextWidth * nextHeight * srcChannels];

			for (size_t y = 0; y < nextHeight; ++y)
			{
				for (size_t x = 0; x < nextWidth; ++x)
				{
					std::vector<float> channelPixel(srcChannels, 0);

					float count = 0;
					for (size_t dy = 0; dy < 2; ++dy)
					{
						for (size_t dx = 0; dx < 2; ++dx)
						{
							size_t srcX = std::min(currentWidth - 1, x * 2 + dx);
							size_t srcY = std::min(currentHeight - 1, y * 2 + dy);
							size_t srcIndex = (srcY * currentWidth + srcX) * srcChannels;

							for (int c = 0; c < srcChannels; ++c)
								channelPixel[c] += currentData[srcIndex + c];

							++count;
						}
					}

					const size_t dstIndex = (y * nextWidth + x) * srcChannels;
					for (int c = 0; c < srcChannels; ++c)
						nextData[dstIndex + c] = channelPixel[c] / count;
				}
			}

			output.push_back({ nextWidth, nextHeight, srcChannels, nextData });

			currentData = std::move(nextData);
			currentWidth = nextWidth;
			currentHeight = nextHeight;
		}
#endif
	}
}