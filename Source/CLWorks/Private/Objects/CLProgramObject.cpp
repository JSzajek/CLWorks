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
		return mpKernel->SetArgument(index, integer);
	return false;
}

bool UCLProgramObject::SetFloatArg(int32 index, float scalar)
{
	if (mpKernel)
		return mpKernel->SetArgument(index, scalar);
	return false;
}

bool UCLProgramObject::SetIntVector2Arg(int32 index, const FIntPoint& vec)
{
	if (mpKernel)
		return mpKernel->SetArgument(index, vec);
	return false;
}

bool UCLProgramObject::SetIntVector4Arg(int32 index, const FIntVector4& vec)
{
	if (mpKernel)
		return mpKernel->SetArgument(index, vec);
	return false;
}

bool UCLProgramObject::SetVector2fArg(int32 index, const FVector2f& vec)
{
	if (mpKernel)
		return mpKernel->SetArgument(index, vec);
	return false;
}

bool UCLProgramObject::SetVector4fArg(int32 index, const FVector4f& vec)
{
	if (mpKernel)
		return mpKernel->SetArgument(index, vec);
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


