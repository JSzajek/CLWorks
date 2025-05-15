#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Core/CLKernel.h"
#include "Core/CLContext.h"

#include "Assets/CLProgramAsset.h"
#include "Objects/CLContextObject.h"
#include "Objects/CLBufferObject.h"
#include "Objects/CLImageObject.h"

#include "CLProgramObject.generated.h"

UCLASS(BlueprintType)
class CLWORKS_API UCLProgramObject : public UObject
{
	friend class UCLWorksLibrary;

	GENERATED_BODY()
protected:
	void Initialize(const TObjectPtr<UCLContextObject>& context,
					const TObjectPtr<UCLProgramAsset>& program, 
					const FString& kernelName);
public:
	bool IsValid() const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="index"></param>
	/// <param name="integer"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Integer Argument")
	bool SetIntArg(int32 index, int32 integer);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="index"></param>
	/// <param name="longinteger"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Int64 Argument")
	bool SetLongIntArg(int32 index, int64 longinteger);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Float Argument")
	bool SetFloatArg(int32 index, float scalar);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Buffer Argument")
	bool SetBufferArg(int32 index, UCLBufferObject* buffer);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Image Argument")
	bool SetImageArg(int32 index, UCLImageObject* image);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	TObjectPtr<UCLProgramAsset> ProgramAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	FString Name;
private:
	std::unique_ptr<OpenCL::Program> mpProgram = nullptr;
	std::unique_ptr<OpenCL::Kernel> mpKernel = nullptr;
};