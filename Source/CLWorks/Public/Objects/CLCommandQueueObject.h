#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Objects/CLContextObject.h"
#include "Core/CLCommandQueue.h"

#include "CLCommandQueueObject.generated.h"

UCLASS(BlueprintType)
class CLWORKS_API UCLCommandQueueObject : public UObject
{
	friend class UCLWorksLibrary;

	GENERATED_BODY()
public:
	bool IsValid() const;
protected:
	void Initialize(const TObjectPtr<UCLContextObject>& context);
private:
	std::unique_ptr<OpenCL::CommandQueue> mpQueue = nullptr;
};