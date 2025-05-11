#include "Core/CLDevice.h"

#include "CLWorksLog.h"

namespace OpenCL
{
	Device::Device()
	{
		cl_platform_id platform;
		cl_device_id dev;
		int err;

		/* Identify a platform */
		err = clGetPlatformIDs(1, &platform, NULL);
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Identify a Platform!"));
			return;
		}

		// Access A Device ------------------------------------------------------------------------
		// GPU
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
		if (err == CL_DEVICE_NOT_FOUND)
		{
			// CPU
			err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
		}
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Access Any Devices!"));
			return;
		}
		// ----------------------------------------------------------------------------------------
		
		mpDevice = dev;
	}

	Device::~Device()
	{
		if (mpDevice)
		{
			clReleaseDevice(mpDevice);
			mpDevice = nullptr;
		}
	}

	bool Device::AreImagesSupported() const
	{
		cl_bool image_support;
		clGetDeviceInfo(mpDevice, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &image_support, NULL);
		if (!image_support)
		{
			UE_LOG(LogCLWorks, Error, TEXT("OpenCL Device Does Not Support Images!"));
			return false;
		}
		return true;
	}

	bool Device::IsExtensionSupported(const std::string& extension) const
	{
		char extensions[1024];
		clGetDeviceInfo(mpDevice, CL_DEVICE_EXTENSIONS, sizeof(extensions), extensions, NULL);
		if (!strstr(extensions, extension.c_str()))
		{
			std::string extenstionsionStr = extension;
			UE_LOG(LogCLWorks, Warning, TEXT("Extension Not Supported: %s"), *FString(extenstionsionStr.c_str()));
			return false;
		}
		return true;
	}
}