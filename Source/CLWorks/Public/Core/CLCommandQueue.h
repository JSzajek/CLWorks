#pragma once

#include "Core/CLContext.h"
#include "Core/CLDevice.h"

#include "Core/CLKernel.h"

#include "Core/CLBuffer.h"
#include "Core/CLImage.h"

namespace OpenCL
{
	class CLWORKS_API CommandQueue
	{
	public:
		CommandQueue();

		CommandQueue(const std::shared_ptr<OpenCL::Context>& context, 
					 const std::shared_ptr<OpenCL::Device>& device);

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

		void ReadBuffer(const OpenCL::Buffer& buffer, 
						size_t data_size, 
						void* output,
						bool isBlocking = true);

		void ReadImageTo(const OpenCL::Image& image,
						 void* output,
						 bool isBlocking = true);

		void* ReadImage(const OpenCL::Image& image,
					    bool isBlocking = true);
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