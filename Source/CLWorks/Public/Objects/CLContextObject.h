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

	const std::shared_ptr<OpenCL::Device> GetDevice() const;
	const std::shared_ptr<OpenCL::Context> GetContext() const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	int32 DeviceIndex = 0;
private:
	std::shared_ptr<OpenCL::Device> mpDevice = nullptr;
	std::shared_ptr<OpenCL::Context> mpContext = nullptr;
};