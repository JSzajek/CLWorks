#include "Objects/CLContextObject.h"

void UCLContextObject::Initialize(uint32_t deviceIndex, 
								  uint32_t platformIndex)
{
	mpDevice = std::move(std::make_unique<OpenCL::Device>(deviceIndex, platformIndex));
	mpContext = std::move(std::make_unique<OpenCL::Context>(*mpDevice));
}

bool UCLContextObject::IsValidDevice() const
{
	if (mpDevice)
		return mpDevice->Get() != nullptr;
	return false;
}

bool UCLContextObject::IsValidContext() const
{
	if (mpContext)
		return mpContext->Get() != nullptr;
	return false;
}

bool UCLContextObject::HasImageSupport() const
{
	if (mpDevice)
		return mpDevice->AreImagesSupported();
	return false;
}

cl_device_id UCLContextObject::GetDevice() const
{
	if (mpDevice)
		return mpDevice->Get();
	return nullptr;
}

cl_context UCLContextObject::GetContext() const
{
	if (mpContext)
		return mpContext->Get();
	return nullptr;
}
