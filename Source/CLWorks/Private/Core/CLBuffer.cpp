#include "Core/CLBuffer.h"

#include "CLWorksLog.h"

namespace OpenCL
{
	Buffer::Buffer()
		: mpBuffer(nullptr)
	{
	}

	Buffer::Buffer(const std::shared_ptr<Context>& context, 
				   void* dataPtr,
				   size_t dataSize,
				   AccessType type)
		: mpBuffer(nullptr)
	{
		Initialize(context->Get(), dataPtr, dataSize, type);
	}

	Buffer::~Buffer()
	{
		if (mpBuffer)
		{
			clReleaseMemObject(mpBuffer);
			mpBuffer = nullptr;
		}
	}

	void Buffer::Initialize(cl_context context, 
							void* dataPtr, 
							size_t dataSize, 
		AccessType type)
	{
		switch (type)
		{
			case AccessType::READ_ONLY:
				mpBuffer = create_input_buffer(context, dataPtr, dataSize);
				break;
			case AccessType::WRITE_ONLY:
				mpBuffer = create_output_buffer(context, dataSize);
				break;
			case AccessType::READ_WRITE:
				mpBuffer = create_inout_buffer(context, dataPtr, dataSize);
				break;
			default:
			{
				UE_LOG(LogCLWorks, Error, TEXT("Invalide Buffer Type: %d"), type);
				break;
			}
		}
	}

	cl_mem Buffer::create_input_buffer(cl_context context, 
									   void* dataPtr, 
									   size_t dataSize)
	{
		cl_int err = -1;
		cl_mem buffer = clCreateBuffer(context,
									   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
									   dataSize,
									   dataPtr,
									   &err);

		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Create Buffer: %d"), err);
			return nullptr;
		}
		return buffer;
	}

	cl_mem Buffer::create_inout_buffer(cl_context context, 
									   void* dataPtr, 
									   size_t dataSize)
	{
		cl_int err = -1;
		cl_mem buffer = clCreateBuffer(context,
									   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
									   dataSize,
									   dataPtr,
									   &err);

		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Create Buffer: %d"), err);
			return nullptr;
		}
		return buffer;
	}

	cl_mem Buffer::create_output_buffer(cl_context context, 
										size_t dataSize)
	{
		cl_int err = -1;
		cl_mem buffer = clCreateBuffer(context,
									   CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
									   dataSize, 
									   NULL, 
									   &err);

		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Create Buffer: %d"), err);
			return nullptr;
		}
		return buffer;
	}
}