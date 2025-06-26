#pragma once

#include "OpenCLLib.h"

#include "Core/CLDevice.h"
#include "Core/CLContext.h"
#include "Core/CLCommandQueue.h"
#include "Core/CLCore.h"

namespace OpenCL
{
	class Kernel;

	class CLWORKS_API Buffer
	{
		friend Kernel;
	public:
		Buffer();

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
	public:
		void Fetch(const OpenCL::CommandQueue& queue,
				   void* src, 
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
		cl_mem mpBuffer;
		void* mpSVMPtr;

		AccessType mAccess = AccessType::INVALID;
		MemoryStrategy mStrategy = MemoryStrategy::INVALID;
	};
}