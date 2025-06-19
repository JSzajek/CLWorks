#include "Core/CLCommandQueue.h"

#include "Profiler/CLProfilerManager.h"

#include "CLWorksLog.h"

namespace OpenCL
{
	CommandQueue::CommandQueue()
		: mpCommandQueue(nullptr),
		mIsValid(false)
	{

	}

	CommandQueue::CommandQueue(const Context& context, 
							   const Device& device)
		: mIsValid(true)
	{
		Initialize(context.Get(), device.Get());
	}

	CommandQueue::CommandQueue(cl_context context, 
							   cl_device_id device)
		: mIsValid(true)
	{
		Initialize(context, device);
	}

	

	CommandQueue::~CommandQueue()
	{
		if (mpCommandQueue)
		{
			clReleaseCommandQueue(mpCommandQueue);
			mpCommandQueue = nullptr;
		}
	}

	void CommandQueue::WaitForFinish() const
	{
		clFinish(mpCommandQueue);
	}

	void CommandQueue::EnqueueRange(const OpenCL::Kernel& kernel,
									size_t work_dim, 
									const size_t* global_work_size,
									const size_t* local_work_size)
	{
		cl_event event;
		int32_t err = clEnqueueNDRangeKernel(mpCommandQueue,
											 kernel.Get(),
											 work_dim,
											 NULL,
											 global_work_size,
											 local_work_size,
											 0,
											 NULL,
											 &event);

		FCLProfilerManager::EnqueueProfiledKernel(*this, kernel, event, work_dim, global_work_size, local_work_size);

		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Enqueue the Kernel: %d"), err);
			mIsValid = false;
		}
	}

	void CommandQueue::ReadBuffer(const OpenCL::Buffer& buffer, 
								  size_t data_size, 
								  void* output,
								  bool isBlocking)
	{
		int32_t err = clEnqueueReadBuffer(mpCommandQueue,
										  buffer.Get(),
										  isBlocking ? CL_TRUE : CL_FALSE,
										  0,
										  data_size,
										  output,
										  0,
										  NULL,
										  NULL);

		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Read the Buffer: %d"), err);
			mIsValid = false;
		}
	}

	void CommandQueue::ReadImageTo(const OpenCL::Image& image, 
								   void* output, 
								   bool isBlocking)
	{
		image.ReadFromCL(&output, 
						 mpCommandQueue, 
						 isBlocking);
	}

	void* CommandQueue::ReadImage(const OpenCL::Image& image, 
								  bool isBlocking)
	{
		void* output = nullptr;
		image.ReadFromCL(&output, mpCommandQueue, isBlocking);
		return output;
	}

	void CommandQueue::Initialize(cl_context context,
								  cl_device_id device)
	{
		mpAttachedDevice = device;

	#ifdef CL_VERSION_2_0

	#if WITH_EDITOR
		cl_command_queue_properties props[3] =
		{
			CL_QUEUE_PROPERTIES,
			CL_QUEUE_PROFILING_ENABLE,
			0
		};
	#else
		cl_command_queue_properties* props = nullptr;
	#endif

		int32_t err = 0;
		mpCommandQueue = clCreateCommandQueueWithProperties(context, device, props, &err);
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Failed Command Queue Creation: %d"), err);
			mIsValid = false;
		}
	#else
		mpCommandQueue = clCreateCommandQueue(context, device, 0, nullptr);
	#endif
	}
}