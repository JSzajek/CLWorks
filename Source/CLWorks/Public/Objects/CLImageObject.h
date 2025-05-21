#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Objects/CLObjectDefines.h"
#include "Objects/CLContextObject.h"
#include "Core/CLImage.h"

#include "CLImageObject.generated.h"

UCLASS(BlueprintType)
class CLWORKS_API UCLImageObject : public UObject
{
	friend class UCLWorksLibrary;

	GENERATED_BODY()
public:
	cl_mem GetData() const { return mpImage ? mpImage->Get() : nullptr; }
protected:
	void Initialize2D(const TObjectPtr<UCLContextObject>& context, 
					  int32_t width,
					  int32_t height,
					  UCLImageFormat format,
					  UCLAccessType type);

	void Initialize2DArray(const TObjectPtr<UCLContextObject>& context,
						   int32_t width,
						   int32_t height,
						   int32_t slices,
						   UCLImageFormat format,
						   UCLAccessType type);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	int32 Width = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	int32 Height = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	int32 Depth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	int32 Layers = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	UCLImageType Type = UCLImageType::Texture2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	TObjectPtr<UTexture> Texture = nullptr;
private:
	std::unique_ptr<OpenCL::Image> mpImage = nullptr;
};