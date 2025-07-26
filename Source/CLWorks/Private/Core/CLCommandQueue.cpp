#include "Core/CLCommandQueue.h"

#include "Core/CLContext.h"
#include "Core/CLKernel.h"

#include "Profiler/CLProfilerManager.h"

#include "CLWorksLog.h"

namespace OpenCL
{
	CommandQueue::CommandQueue()
		: mpCommandQueue(nullptr),
		mIsValid(false)
	{

	}

	CommandQueue::CommandQueue(const OpenCL::ContextPtr& context, 
							   const OpenCL::DevicePtr& device)
		: mpCommandQueue(nullptr),
		mpContext(context),
		mpAttachedDevice(device),
		mIsValid(true)
	{
		Initialize(context->Get(), device->Get());
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

	void CommandQueue::Initialize(cl_context context,
								  cl_device_id device)
	{
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