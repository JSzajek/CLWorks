#include "Core/CLDevice.h"

#include "CLWorksLog.h"

namespace OpenCL
{
	Device::Device(uint32_t deviceIndex,
				   uint32_t platformIndex)
	{
		cl_platform_id platform = nullptr;
		cl_device_id dev = nullptr;
		int32_t err = 0;

		// Identify the platforms -----------------------------------------------------------------
		cl_uint numPlatforms = 0;
		clGetPlatformIDs(0, nullptr, &numPlatforms);
		if (numPlatforms == 0) 
			return;

		if (numPlatforms < platformIndex)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Find Platform with Index: %d"), platformIndex);
			return;
		}

		std::vector<cl_platform_id> platforms(numPlatforms, nullptr);
		err = clGetPlatformIDs(numPlatforms, platforms.data(), NULL);
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Identify a Platform!"));
			return;
		}

		platform = platforms[platformIndex];
		// ----------------------------------------------------------------------------------------


		// Access A Device ------------------------------------------------------------------------
		cl_uint numDevices = 0;
		clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
		if (numDevices == 0)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Couldn't Find a GPU and Falling Back to CPU."));

			err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
			if (err < 0)
			{
				UE_LOG(LogCLWorks, Error, TEXT("Couldn't Access Any Devices!"));
				return;
			}
		}

		if (numDevices < deviceIndex)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Find Device with Index: %d"), deviceIndex);
			return;
		}

		std::vector<cl_device_id> devices(numDevices, nullptr);
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices.data(), NULL);
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Retrieve Devices!"));
			return;
		}

		mpDevice = devices[deviceIndex];
		// ----------------------------------------------------------------------------------------
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
			return false;
		}
		return true;
	}
}