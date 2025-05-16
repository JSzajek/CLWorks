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
public:
	/// <summary>
	/// Retrieves whether the program is valid.
	/// </summary>
	/// <returns>True if the program is valid</returns>
	bool IsValid() const;

	/// <summary>
	/// Sets a integer argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="integer">The integer</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Integer Argument")
	bool SetIntArg(int32 index, int32 integer);

	/// <summary>
	/// Sets a long integer argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="longinteger">The long integer</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Int64 Argument")
	bool SetLongIntArg(int32 index, int64 longinteger);

	/// <summary>
	/// Sets a float argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="scalar">The float</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Float Argument")
	bool SetFloatArg(int32 index, float scalar);

	/// <summary>
	/// Sets a buffer argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="buffer">The buffer</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Buffer Argument")
	bool SetBufferArg(int32 index, UCLBufferObject* buffer);

	/// <summary>
	/// Sets a image argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="image">The image</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Image Argument")
	bool SetImageArg(int32 index, UCLImageObject* image);
protected:
	void Initialize(const TObjectPtr<UCLContextObject>& context,
					const TObjectPtr<UCLProgramAsset>& program, 
					const FString& kernelName);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	TObjectPtr<UCLProgramAsset> ProgramAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CLWorks")
	FString Name;
private:
	std::unique_ptr<OpenCL::Program> mpProgram = nullptr;
	std::unique_ptr<OpenCL::Kernel> mpKernel = nullptr;
};