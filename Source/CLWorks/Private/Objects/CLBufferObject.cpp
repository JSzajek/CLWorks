#include "Objects/CLBufferObject.h"

void UCLBufferObject::Initialize(const TObjectPtr<UCLContextObject>& context,
								 void* dataPtr,
								 int32 dataSize,
								 UCLAccessType type,
								 UCLMemoryStrategy strategy)
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

	OpenCL::MemoryStrategy memory = OpenCL::MemoryStrategy::STREAM;
	switch (strategy)
	{
		case UCLMemoryStrategy::COPY_ONCE:
			memory = OpenCL::MemoryStrategy::COPY_ONCE;
			break;
		case UCLMemoryStrategy::STREAM:
			memory = OpenCL::MemoryStrategy::STREAM;
			break;
		case UCLMemoryStrategy::ZERO_COPY:
			memory = OpenCL::MemoryStrategy::ZERO_COPY;
			break;
	}

	mpBuffer = std::make_shared<OpenCL::Buffer>(context->GetDevice(), context->GetContext(), dataPtr, dataSize, access, memory);
}