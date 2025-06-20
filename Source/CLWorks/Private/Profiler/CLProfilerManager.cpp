#include "Profiler/CLProfilerManager.h"

#include "CLWorksLog.h"

#include "Profiler/CLStats.h"

FCLHardwareMetrics FCLProfilerManager::HardwareMetrics = {};

TArray<FKernelProfile> FCLProfilerManager::ActiveKernels = {};
TArray<FKernelProfile> FCLProfilerManager::CompletedKernels = {};

std::mutex mProfileMutex = {};

FCLProfilerManager::FCLProfilerManager()
{
	// Profile default hardware statistics
	OpenCL::Device device;

	clGetDeviceInfo(device.Get(), CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(HardwareMetrics.MaxComputeUnits), &HardwareMetrics.MaxComputeUnits, nullptr);
	clGetDeviceInfo(device.Get(), CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(HardwareMetrics.MaxWorkGroupSize), &HardwareMetrics.MaxWorkGroupSize, nullptr);
	clGetDeviceInfo(device.Get(), CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(HardwareMetrics.GlobalMemSize), &HardwareMetrics.GlobalMemSize, nullptr);
	clGetDeviceInfo(device.Get(), CL_DEVICE_LOCAL_MEM_SIZE, sizeof(HardwareMetrics.LocalMemSize), &HardwareMetrics.LocalMemSize, nullptr);

	SET_DWORD_STAT(STAT_OpenCL_TotalComputeUnits, HardwareMetrics.MaxComputeUnits);
	SET_DWORD_STAT(STAT_OpenCL_TotalWorkgroups, HardwareMetrics.MaxWorkGroupSize);

	SET_MEMORY_STAT(STAT_OpenCL_HardwareGlobalMemory, HardwareMetrics.GlobalMemSize);
	SET_MEMORY_STAT(STAT_OpenCL_HardwareLocalMemory, HardwareMetrics.LocalMemSize);
}

FCLProfilerManager::~FCLProfilerManager()
{
}


void FCLProfilerManager::Tick(float DeltaTime)
{
	PollEvents();
	UpdateStats();
}

TStatId FCLProfilerManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FCLProfilerManager, STATGROUP_Tickables);
}

void FCLProfilerManager::EnqueueProfiledKernel(const OpenCL::CommandQueue& queue,
											   const OpenCL::Kernel& kernel,
											   const OpenCL::Event& event,
											   size_t work_dim,
											   const size_t* global_work_size,
											   const size_t* local_work_size)
{
	const std::scoped_lock lock(mProfileMutex);

	FKernelProfile Profile;
	Profile.mName = kernel.GetName();
	Profile.mEvent = event.Get();
	
	OpenCL::DevicePtr device_ptr = queue.GetDevicePtr();
	if (!device_ptr)
	{
		UE_LOG(LogCLWorks, Warning, TEXT("Failed To Enqueue Kernel: %s"), *FString(kernel.GetName().c_str()));
		return;
	}

	clGetKernelWorkGroupInfo(kernel, *device_ptr, CL_KERNEL_WORK_GROUP_SIZE, sizeof(Profile.KernelWorkGroupSize), &Profile.KernelWorkGroupSize, nullptr);
	clGetKernelWorkGroupInfo(kernel, *device_ptr, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(Profile.PreferredWorkGroupMultiple), &Profile.PreferredWorkGroupMultiple, nullptr);
	clGetKernelWorkGroupInfo(kernel, *device_ptr, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(Profile.CompiledWorkGroupSize), &Profile.CompiledWorkGroupSize, nullptr);
	clGetKernelWorkGroupInfo(kernel, *device_ptr, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(Profile.PrivateMemSize), &Profile.PrivateMemSize, nullptr);
	clGetKernelWorkGroupInfo(kernel, *device_ptr, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(Profile.LocalMemSizeUsed), &Profile.LocalMemSizeUsed, nullptr);

	ActiveKernels.Add(Profile);
}

void FCLProfilerManager::PollEvents()
{
	const std::scoped_lock lock(mProfileMutex);

	for (int32 i = ActiveKernels.Num() - 1; i >= 0; --i)
	{
		cl_int eventStatus;
		clGetEventInfo(ActiveKernels[i].mEvent, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(eventStatus), &eventStatus, nullptr);

		if (eventStatus == CL_COMPLETE)
		{
			clGetEventProfilingInfo(ActiveKernels[i].mEvent, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ActiveKernels[i].mStartTimeNs, nullptr);
			clGetEventProfilingInfo(ActiveKernels[i].mEvent, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ActiveKernels[i].mEndTimeNs, nullptr);

			clReleaseEvent(ActiveKernels[i].mEvent);
			CompletedKernels.Add(ActiveKernels[i]);
			ActiveKernels.RemoveAt(i);
		}
	}
}

void FCLProfilerManager::UpdateStats()
{
	const std::scoped_lock lock(mProfileMutex);

	SET_DWORD_STAT(STAT_OpenCL_ActiveKernels, ActiveKernels.Num());

	if (CompletedKernels.Num() > 0)
	{
		float TotalTime_ms = 0.0f;
		int32 TotalGroups = 0;

		for (const FKernelProfile& Profile : CompletedKernels)
		{
			TotalTime_ms += Profile.GetDurationMs();
			TotalGroups += Profile.GetWorkGroupCount();
		}

		SET_FLOAT_STAT(STAT_OpenCL_KernelTime, TotalTime_ms);

		CompletedKernels.Empty();
	}
	else
	{
		SET_FLOAT_STAT(STAT_OpenCL_KernelTime, 0.0f);
	}
}

