#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Core/CLKernel.h"

#include "CLProgramAsset.generated.h"

UCLASS(BlueprintType)
class CLWORKS_API UCLProgramAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	FString ProgramName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	FString SourceCode;
};