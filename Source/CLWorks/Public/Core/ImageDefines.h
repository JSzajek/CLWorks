#pragma once

struct Mip
{
	size_t mWidth			= 0;
	size_t mHeight			= 0;
	size_t mSlices			= 0;
	uint8_t mChannels		= 0;
	void* mPixels			= nullptr;
};