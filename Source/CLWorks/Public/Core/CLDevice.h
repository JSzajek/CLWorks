#pragma once

#include "OpenCLLib.h"

#include <string>

namespace OpenCL
{
	class Device
	{
	public:
		Device();
		~Device();
	public:
		bool AreImagesSupported() const;
		bool IsExtensionSupported(const std::string& extension) const;
	public:
		cl_device_id Get() const { return mpDevice; }
	private:
		cl_device_id mpDevice;
	};
}