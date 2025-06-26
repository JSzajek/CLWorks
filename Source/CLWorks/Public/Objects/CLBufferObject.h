#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Objects/CLObjectDefines.h"
#include "Objects/CLContextObject.h"
#include "Core/CLBuffer.h"

#include "CLBufferObject.generated.h"

UCLASS(BlueprintType)
class CLWORKS_API UCLBufferObject : public UObject
{
	friend class UCLWorksLibrary;

	GENERATED_BODY()
public:
	cl_mem GetData() const { return mpBuffer ? mpBuffer->Get() : nullptr; }
protected:
	void Initialize(const TObjectPtr<UCLContextObject>& context,
					void* dataPtr,
					int32 dataSize,
					UCLAccessType type,
					UCLMemoryStrategy strategy);
private:
	std::shared_ptr<OpenCL::Buffer> mpBuffer;
};