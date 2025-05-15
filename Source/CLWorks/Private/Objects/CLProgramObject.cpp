#include "Objects/CLProgramObject.h"

void UCLProgramObject::Initialize(const TObjectPtr<UCLContextObject>& context,
								  const TObjectPtr<UCLProgramAsset>& program, 
								  const FString& kernelName)
{
	ProgramAsset = program;
	Name = kernelName;

	mpProgram = std::make_unique<OpenCL::Program>(context->GetContext(), context->GetDevice());

	const std::string programString(TCHAR_TO_UTF8(*program->SourceCode));
	if (!mpProgram->ReadFromString(programString))
		return;
	
	const std::string name(TCHAR_TO_UTF8(*kernelName));
	mpKernel = std::make_unique<OpenCL::Kernel>(*mpProgram, name);
}

bool UCLProgramObject::IsValid() const
{
	return mpProgram && mpKernel;
}

bool UCLProgramObject::SetIntArg(int32 index, int32 integer)
{
	if (mpKernel)
	{
		return mpKernel->SetArgument<int32>(index, integer);
	}
	return false;
}

bool UCLProgramObject::SetLongIntArg(int32 index, int64 longinteger)
{
	if (mpKernel)
	{
		return mpKernel->SetArgument<int64>(index, longinteger);
	}
	return false;
}

bool UCLProgramObject::SetFloatArg(int32 index, float scalar)
{
	if (mpKernel)
	{
		return mpKernel->SetArgument<float>(index, scalar);
	}
	return false;
}

bool UCLProgramObject::SetBufferArg(int32 index, UCLBufferObject* buffer)
{
	if (mpKernel)
	{
		return mpKernel->SetArgument<cl_mem>(index, buffer->GetData());
	}
	return false;
}

bool UCLProgramObject::SetImageArg(int32 index, UCLImageObject* image)
{
	if (mpKernel)
	{
		return mpKernel->SetArgument<cl_mem>(index, image->GetData());
	}
	return false;
}


