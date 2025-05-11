#include "Core/CLKernel.h"

#include "CLWorksLog.h"

namespace OpenCL
{
	Kernel::Kernel(const Program& program, 
				   const std::string& kernalName)
		: mIsValid(true)
	{
		Initialize(program.Get(), kernalName);
	}

	Kernel::Kernel(cl_program program, 
				   const std::string& kernalName)
		: mIsValid(true)
	{
		Initialize(program, kernalName);
	}

	Kernel::~Kernel()
	{
		if (mpKernel)
		{
			clReleaseKernel(mpKernel);
			mpKernel = nullptr;
		}
	}

	void Kernel::SetArgument(cl_uint arg_index, size_t arg_size, const void* arg_value)
	{
		if (!mIsValid)
			return;

		cl_int err = clSetKernelArg(mpKernel, arg_index, arg_size, arg_value);
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Create Kernel Argument!: %d"), err);
			mIsValid = false;
		}
	}

	void Kernel::Initialize(cl_program program,
							const std::string& kernalName)
	{
		cl_int err = 0;

		cl_kernel kernel = clCreateKernel(program, kernalName.c_str(), &err);
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Create A Kernel!: %d"), err);
			mIsValid = false;
		}
		mpKernel = kernel;
	}
}