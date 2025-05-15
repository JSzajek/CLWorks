#include "Core/CLContext.h"

#include "CLWorksLog.h"

#include <vector>
#include <cstdlib>

namespace OpenCL
{
	Context::Context()
		: mpContext(nullptr)
	{
	}

	Context::Context(const Device& device, 
					 const ContextProperties& properties)
	{
		Initialize(device.Get(), properties);
	}

	Context::Context(cl_device_id device, 
					 const ContextProperties& properties)
	{
		Initialize(device, properties);
	}

	Context::~Context()
	{
		if (mpContext)
		{
			clReleaseContext(mpContext);
		}
	}

	void Context::PrintSupportedImageFormats(cl_mem_flags mem_flags)
	{
		cl_int err = 0;
		cl_uint num_formats = 0;

		// Query The Number of Supported Formats
		err = clGetSupportedImageFormats(mpContext, mem_flags, CL_MEM_OBJECT_IMAGE2D, 0, NULL, &num_formats);
		if (err != CL_SUCCESS) 
		{
			UE_LOG(LogCLWorks, Error, TEXT("Querying Supported Image Formats Count!"));
			return;
		}

		if (num_formats == 0) 
		{
			UE_LOG(LogCLWorks, Log, TEXT("No Supported Image Formats Found."));
			return;
		}

		// Allocate space for format list
		cl_image_format* formats = (cl_image_format*)malloc(num_formats * sizeof(cl_image_format));

		// Retrieve the list of supported formats
		err = clGetSupportedImageFormats(mpContext, mem_flags, CL_MEM_OBJECT_IMAGE2D, num_formats, formats, NULL);
		if (err != CL_SUCCESS) 
		{
			UE_LOG(LogCLWorks, Error, TEXT("Error Retrieving Supported Image Formats: %d"), err);
			free(formats);
			return;
		}

		// Print supported formats
		UE_LOG(LogCLWorks, Log, TEXT("Supported OpenCL Image Formats:"));
		for (cl_uint i = 0; i < num_formats; i++)
		{
			UE_LOG(LogCLWorks, Log, TEXT("  [%d] Channel Order = %d, Data Type = %d"), i, 
																					   formats[i].image_channel_order, 
																					   formats[i].image_channel_data_type);
		}
		free(formats);
	}

	void Context::Initialize(cl_device_id device, 
							 const ContextProperties& properties)
	{
		if (device == nullptr)
			return;

		cl_int err = 0;

		cl_platform_id platform;
		clGetPlatformIDs(1, &platform, NULL);

		// TODO:: Implement Interop
		std::vector<cl_context_properties> props;

		mpContext = clCreateContext(props.data(), 1, &device, NULL, NULL, &err);
		if (err < 0)
		{
			perror("Couldn't create a context!");
		}
	}
}