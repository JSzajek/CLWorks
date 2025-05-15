#include "Objects/CLCommandQueueObject.h"

void UCLCommandQueueObject::Initialize(const TObjectPtr<UCLContextObject>& context)
{
	mpQueue = std::make_unique<OpenCL::CommandQueue>(context->GetContext(), context->GetDevice());
}

bool UCLCommandQueueObject::IsValid() const
{
	if (mpQueue)
		return mpQueue->IsValid();
	return false;
}