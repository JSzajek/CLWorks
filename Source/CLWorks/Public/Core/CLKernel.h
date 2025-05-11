#pragma once

#include "Core/CLProgram.h"

#include <string>

namespace OpenCL
{
	class Kernel
	{
	public:
		Kernel(const OpenCL::Program& program, 
			   const std::string& kernalName);

		Kernel(cl_program program, 
			   const std::string& kernalName);

		~Kernel();
	public:
		cl_kernel Get() const { return mpKernel; };

		bool IsValid() const { return mIsValid; }

		template<typename T>
		void SetArgument(cl_uint arg_index,
						 const T& arg_value)
		{
			SetArgument(arg_index, sizeof(T), &arg_value);
		}

		void SetArgument(cl_uint arg_index,
						 size_t arg_size,
						 const void* arg_value);
	private:
		void Initialize(cl_program program, 
						const std::string& kernalName);
	private:
		cl_kernel mpKernel;
		bool mIsValid;
	};
}