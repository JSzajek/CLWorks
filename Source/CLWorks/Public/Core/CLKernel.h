#pragma once

#include "Core/CLProgram.h"

#include <string>

namespace OpenCL
{
	class CLWORKS_API Kernel
	{
	public:
		Kernel();

		Kernel(const Program& program,
			   const std::string& kernalName);

		~Kernel();
	public:
		operator cl_kernel() const { return mpKernel; }
		inline cl_kernel Get() const { return mpKernel; };

		inline bool IsValid() const { return mIsValid; }

		inline std::string GetName() const { return mName; }

		template<typename T>
		bool SetArgument(cl_uint arg_index,
						 const T& arg_value)
		{
			return SetArgument(arg_index, sizeof(T), &arg_value);
		}

		bool SetArgument(cl_uint arg_index,
						 size_t arg_size,
						 const void* arg_value);
	private:
		void Initialize(cl_program program, 
						const std::string& kernalName);
	private:
		std::string mName;
		cl_kernel mpKernel;
		bool mIsValid;
	};
}