#include "Core/CLBuffer.h"

#include "CLWorksLog.h"

namespace OpenCL
{
	Buffer::Buffer()
		: mpBuffer(nullptr)
	{
	}

	Buffer::Buffer(const std::shared_ptr<Device>& device,
				   const std::shared_ptr<Context>& context,
				   void* dataPtr,
				   size_t dataSize,
				   AccessType access,
				   MemoryStrategy strategy)
		: mpBuffer(nullptr),
		mpSVMPtr(nullptr),
		mAccess(access),
		mStrategy(strategy)
	{
		if (strategy == MemoryStrategy::ZERO_COPY)
		{
			SVMSupport svmSupport = device->GetSVMSupported();
			if (svmSupport == SVMSupport::None)
			{
				// Fallback to streaming memory strategy on devices with no zero-copy support.
				UE_LOG(LogCLWorks, Log, TEXT("Falling Back To STREAM Memory Strategy!"));

				mStrategy = MemoryStrategy::STREAM;
			}
		}

		cl_mem_flags flags = 0;
		switch (mAccess)
		{
			case AccessType::READ_ONLY:  
				flags |= CL_MEM_READ_ONLY; 
				break;
			case AccessType::WRITE_ONLY:
			{
				flags |= CL_MEM_WRITE_ONLY; 
				break;
			}
			case AccessType::READ_WRITE: 
				flags |= CL_MEM_READ_WRITE; 
				break;
		}

		switch (mStrategy)
		{
			case MemoryStrategy::COPY_ONCE:
			{
				flags |= CL_MEM_COPY_HOST_PTR;
				mpBuffer = CreateBuffer(context->Get(), flags, dataPtr, dataSize);
				break;
			}
			case MemoryStrategy::STREAM:
			{
				if (dataPtr)
					flags |= CL_MEM_COPY_HOST_PTR;

				mpBuffer = CreateBuffer(context->Get(), flags, dataPtr, dataSize);
				break;
			}
			case MemoryStrategy::ZERO_COPY:
			{
				mpSVMPtr = clSVMAlloc(context->Get(), CL_MEM_READ_WRITE, dataSize, 0);

				if (dataPtr)
				{
					OpenCL::CommandQueue localqueue(context, device);
					Upload(localqueue, dataPtr, dataSize);
				}
				break;
			}
		}
	}

	Buffer::~Buffer()
	{
		if (mpBuffer)
		{
			clReleaseMemObject(mpBuffer);
			mpBuffer = nullptr;
		}
	}

	void Buffer::Fetch(const OpenCL::CommandQueue& queue,
					   void* output, 
					   size_t size, 
					   size_t offset)
	{
		// TODO:: Add Support For Asynchronous Fetching.

		switch (mStrategy)
		{
			case MemoryStrategy::COPY_ONCE:
			{
				clEnqueueReadBuffer(queue, 
									mpBuffer, 
									CL_TRUE, 
									offset, 
									size, 
									output, 
									0, 
									nullptr, 
									nullptr);
				break;
			}
			case MemoryStrategy::STREAM:
			{
				cl_int err;
				void* hostPtr = clEnqueueMapBuffer(queue, 
												   mpBuffer, 
												   CL_TRUE, 
												   CL_MAP_READ, 
												   0, 
												   size, 
												   0,
												   nullptr, 
												   nullptr, 
												   &err);

				if (!hostPtr)
				{
					UE_LOG(LogCLWorks, Error, TEXT("Failed to Map Buffer: %d"), err);
					return;
				}

				std::memcpy(output, (uint8_t*)hostPtr + offset, size);

				clEnqueueUnmapMemObject(queue, 
										mpBuffer, 
										hostPtr, 
										0, 
										nullptr, 
										nullptr);
				break;
			}
			case MemoryStrategy::ZERO_COPY:
			{
				clEnqueueSVMMap(queue, 
								CL_TRUE, 
								CL_MAP_WRITE, 
								mpSVMPtr, 
								size, 
								0,
								nullptr, 
								nullptr);

				uint8_t* pt = (uint8_t*)mpSVMPtr + offset;
				memcpy(output, pt, size);

				clEnqueueSVMUnmap(queue, 
									mpSVMPtr, 
									0, 
									nullptr,
									nullptr);
				break;
			}
		}
	}

	void Buffer::Upload(const OpenCL::CommandQueue& queue,
						const void* src, 
						size_t size, 
						size_t offset)
	{
		switch (mStrategy)
		{
			case MemoryStrategy::COPY_ONCE:
			{
				check(false);
				UE_LOG(LogCLWorks, Error, TEXT("Attempted to Upload Into a COPY_ONCE buffer."));
				break;
			}
			case MemoryStrategy::STREAM:
			{
				cl_int err;
				void* hostPtr = clEnqueueMapBuffer(queue, 
												   mpBuffer, 
												   CL_TRUE, 
												   CL_MAP_WRITE, 
												   0, 
												   size, 
												   0, 
												   nullptr, 
												   nullptr, 
												   &err);

				if (!hostPtr)
				{
					UE_LOG(LogCLWorks, Error, TEXT("Failed to Map Buffer: %d"), err);
					return;
				}

				std::memcpy((uint8_t*)hostPtr + offset, src, size);

				clEnqueueUnmapMemObject(queue, 
										mpBuffer, 
										hostPtr, 
										0, 
										nullptr, 
										nullptr);
				break;
			}
			case MemoryStrategy::ZERO_COPY:
			{
				clEnqueueSVMMap(queue, 
								CL_TRUE, 
								CL_MAP_WRITE, 
								mpSVMPtr, 
								size, 
								0, 
								nullptr, 
								nullptr);

				memcpy((uint8_t*)mpSVMPtr + offset, src, size);

				clEnqueueSVMUnmap(queue, 
								  mpSVMPtr, 
								  0, 
								  nullptr, 
								  nullptr);
				break;
			}
		}
	}

	cl_mem Buffer::CreateBuffer(cl_context context,
							    cl_mem_flags flags,
							    void* dataPtr, 
							    size_t dataSize)
	{
		cl_int err = -1;
		cl_mem buffer = clCreateBuffer(context,
									   flags,
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

	bool Buffer::AttachToKernel(cl_kernel kernel,
								cl_uint arg_index) const
	{
		cl_int err = -1;
		if (mStrategy == MemoryStrategy::ZERO_COPY)
			err = clSetKernelArgSVMPointer(kernel, arg_index, mpSVMPtr);
		else
			err = clSetKernelArg(kernel, arg_index, sizeof(cl_mem), &mpBuffer);


		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Create Kernel Argument!: %d"), err);
			return false;
		}

		return true;
	}
}