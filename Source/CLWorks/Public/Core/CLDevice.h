#pragma once

#include "OpenCLLib.h"

#include <string>

namespace OpenCL
{
	class CLWORKS_API Device
	{
	public:
		Device(uint32_t deviceIndex = 0, 
			   uint32_t platformIndex = 0);

		~Device();
	public:
		bool AreImagesSupported() const;
		bool IsExtensionSupported(const std::string& extension) const;
	public:
		operator cl_device_id() const { return mpDevice; }
		cl_device_id Get() const { return mpDevice; }
	private:
		cl_device_id mpDevice;
	};

	using DevicePtr = std::shared_ptr<OpenCL::Device>;

	inline static DevicePtr MakeDevice(uint32_t deviceIndex = 0, 
									   uint32_t platformIndex = 0)
	{
		return std::make_shared<OpenCL::Device>(deviceIndex, platformIndex);
	}
}