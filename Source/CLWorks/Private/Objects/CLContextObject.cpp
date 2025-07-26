#include "Objects/CLContextObject.h"

void UCLContextObject::Initialize(uint32_t deviceIndex, 
								  uint32_t platformIndex)
{
	mpDevice = OpenCL::MakeDevice(deviceIndex, platformIndex);
	mpContext = OpenCL::MakeContext(mpDevice);
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

const OpenCL::DevicePtr UCLContextObject::GetDevice() const
{
	if (mpDevice)
		return mpDevice;
	return nullptr;
}

const OpenCL::ContextPtr UCLContextObject::GetContext() const
{
	if (mpContext)
		return mpContext;
	return nullptr;
}
