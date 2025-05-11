#pragma once

#include "Core/CLContext.h"
#include "Core/CLDevice.h"

#include "Core/CLKernel.h"

#include "Core/CLBuffer.h"
#include "Core/CLImage.h"

namespace OpenCL
{
	class CommandQueue
	{
	public:
		CommandQueue(const OpenCL::Context& context, 
					 const OpenCL::Device& device);

		CommandQueue(cl_context context, 
					 cl_device_id device);

		~CommandQueue();
	public:
		cl_command_queue Get() const { return mpCommandQueue; };

		bool IsValid() const { return mIsValid; }

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
		bool mIsValid;
	};
}