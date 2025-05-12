#pragma once

#include "OpenCLLib.h"

#include "Core/CLCore.h"
#include "Core/CLContext.h"

namespace OpenCL
{
	class CLWORKS_API Buffer
	{
	public:
		Buffer(cl_context context,
			   void* dataPtr,
			   size_t dataSize,
			   AccessType type);

		Buffer(const OpenCL::Context& context,
			   void* dataPtr,
			   size_t dataSize,
			   AccessType type);

		~Buffer();
	public:
		operator cl_mem() const { return mpBuffer; }

		cl_mem Get() const { return mpBuffer; }
	private:
		void Initialize(cl_context context,
						void* dataPtr,
						size_t dataSize,
						AccessType type);

		cl_mem create_input_buffer(cl_context context, 
								   void* dataPtr, 
								   size_t dataSize);

		cl_mem create_inout_buffer(cl_context context, 
								   void* dataPtr, 
								   size_t dataSize);

		cl_mem create_output_buffer(cl_context context, 
									size_t dataSize);
	private:
		cl_mem mpBuffer;
	};
}