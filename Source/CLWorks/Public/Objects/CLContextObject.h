#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Core/CLDevice.h"
#include "Core/CLContext.h"

#include "CLContextObject.generated.h"

UCLASS(BlueprintType)
class CLWORKS_API UCLContextObject : public UObject
{
	friend class UCLWorksLibrary;

	GENERATED_BODY()
protected:
	void Initialize(uint32_t deviceIndex = 0, uint32_t platformIndex = 0);
public:
	bool IsValid() const { return IsValidDevice() && IsValidContext(); }
	bool IsValidDevice() const;
	bool IsValidContext() const;

	bool HasImageSupport() const;

	const OpenCL::DevicePtr GetDevice() const;
	const OpenCL::ContextPtr GetContext() const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	int32 DeviceIndex = 0;
private:
	OpenCL::DevicePtr mpDevice = nullptr;
	OpenCL::ContextPtr mpContext = nullptr;
};