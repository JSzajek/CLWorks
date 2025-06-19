#pragma once

#include "Tickable.h"

#include "Core/CLCommandQueue.h"
#include "Core/CLKernel.h"
#include "Core/CLEvent.h"

#include <mutex>

struct CLWORKS_API FCLHardwareMetrics
{
	uint32_t MaxComputeUnits = 0;
	size_t MaxWorkGroupSize = 0;
	cl_ulong GlobalMemSize = 0;
	cl_ulong LocalMemSize = 0;
};

struct CLWORKS_API FKernelProfile
{
public:
	float GetDurationMs() const
	{
		return (mEndTimeNs > mStartTimeNs) ? float(mEndTimeNs - mStartTimeNs) * 1e-6f : 0.0f;
	}

	int32 GetWorkGroupCount() const
	{
		return (mLocalWorkSize > 0) ? mGlobalWorkSize / mLocalWorkSize : 0;
	}
public:
	std::string mName;
	cl_event mEvent;
	uint64_t mStartTimeNs = 0;
	uint64_t mEndTimeNs = 0;

	size_t KernelWorkGroupSize = 0;
	size_t CompiledWorkGroupSize[3] = {};
	size_t PreferredWorkGroupMultiple = 0;

	uint64_t mGlobalWorkSize = 0;
	uint64_t mLocalWorkSize = 0;

	cl_ulong PrivateMemSize = 0;
	cl_ulong LocalMemSizeUsed = 0;
};

class CLWORKS_API FCLProfilerManager : public FTickableGameObject
{
public:
	FCLProfilerManager();
	~FCLProfilerManager();
public:
	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickableWhenPaused() const override { return true; }

	virtual bool IsTickableInEditor() const override { return true; }

	virtual bool IsTickable() const override { return true; }

	virtual TStatId GetStatId() const override;
public:
	static void EnqueueProfiledKernel(const OpenCL::CommandQueue& queue,
									  const OpenCL::Kernel& kernel,
									  const OpenCL::Event& event,
									  size_t work_dim,
									  const size_t* global_work_size,
									  const size_t* local_work_size);
private:
	void PollEvents();

	void UpdateStats();
private:
	static FCLHardwareMetrics HardwareMetrics;

	static TArray<FKernelProfile> ActiveKernels;

	static TArray<FKernelProfile> CompletedKernels;
};