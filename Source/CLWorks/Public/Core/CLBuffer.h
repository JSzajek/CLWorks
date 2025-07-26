#pragma once

#include "OpenCLLib.h"

#include "Core/CLCore.h"
#include "Core/CLDevice.h"
#include "Core/CLContext.h"
#include "Core/CLCommandQueue.h"
#include "Core/CLEvent.h"

namespace OpenCL
{
	class Kernel;

	class CLWORKS_API Buffer
	{
		friend Kernel;
	public:
		Buffer() = default;

		Buffer(const std::shared_ptr<Device>& device,
			   const std::shared_ptr<Context>& context,
			   void* dataPtr,
			   size_t dataSize,
			   AccessType access,
			   MemoryStrategy strategy);

		~Buffer();
	public:
		operator cl_mem() const { return mpBuffer; }
		cl_mem Get() const { return mpBuffer; }

		bool IsValid() const { return mpBuffer || mpSVMPtr; }

		size_t Size() const { return mDataSize; }
	public:
		void Fetch(const OpenCL::CommandQueue& queue,
				   void* output,
				   size_t size, 
				   size_t offset = 0);

		void FetchAsync(const std::shared_ptr<OpenCL::CommandQueue>& queue,
						const std::function<void()>& callback,
						void* output, 
						size_t size, 
						size_t offset = 0);

		void Upload(const OpenCL::CommandQueue& queue,
				    const void* src, 
					size_t size, 
					size_t offset = 0);
	private:
		cl_mem CreateBuffer(cl_context context,
						    cl_mem_flags flags,
						    void* dataPtr,
						    size_t dataSize);

		bool AttachToKernel(cl_kernel kernel, 
							cl_uint arg_index) const;
	private:
		size_t mDataSize = 0;

		cl_mem mpBuffer = nullptr;
		void* mpSVMPtr = nullptr;

		AccessType mAccess = AccessType::INVALID;
		MemoryStrategy mStrategy = MemoryStrategy::INVALID;

		Event mReadbackEvent;
	};
}