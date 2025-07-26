#pragma once

#include "Core/CLDevice.h"
#include "Core/CLContext.h"

#include <memory>

namespace OpenCL
{
	class Context;
	class Kernel;

	class CLWORKS_API CommandQueue
	{
	public:
		CommandQueue();

		CommandQueue(const OpenCL::ContextPtr& context, 
					 const OpenCL::DevicePtr& device);

		~CommandQueue();
	public:
		operator cl_command_queue() const { return mpCommandQueue; };
		cl_command_queue Get() const { return mpCommandQueue; };

		inline DevicePtr GetDevicePtr() const { return mpAttachedDevice.lock(); }
	public:
		bool IsValid() const { return mIsValid; }

		void WaitForFinish() const;

		void EnqueueRange(const OpenCL::Kernel& kernel, 
						  size_t work_dim, 
						  const size_t* global_work_size,
						  const size_t* local_work_size = nullptr);
	private:
		void Initialize(cl_context context, 
						cl_device_id device);
	private:
		cl_command_queue mpCommandQueue;

		std::weak_ptr<OpenCL::Context> mpContext;
		std::weak_ptr<OpenCL::Device> mpAttachedDevice;
		bool mIsValid;
	};
}