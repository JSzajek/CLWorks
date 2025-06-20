#include "Objects/CLBufferObject.h"

void UCLBufferObject::Initialize(const TObjectPtr<UCLContextObject>& context,
								 void* dataPtr,
								 int32 dataSize,
								 UCLAccessType type)
{
	OpenCL::AccessType access = OpenCL::AccessType::READ_WRITE;
	switch (type)
	{
		case UCLAccessType::READ_ONLY:
			access = OpenCL::AccessType::READ_ONLY;
			break;
		case UCLAccessType::WRITE_ONLY:
			access = OpenCL::AccessType::WRITE_ONLY;
			break;
		case UCLAccessType::READ_WRITE:
			access = OpenCL::AccessType::READ_WRITE;
			break;
	}

	mpBuffer = std::make_shared<OpenCL::Buffer>(context->GetContext(), dataPtr, dataSize, access);
}