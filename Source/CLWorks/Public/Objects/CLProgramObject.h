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
	/// Sets a float argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="scalar">The float</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Float Argument")
	bool SetFloatArg(int32 index, float scalar);

	/// <summary>
	/// Sets a integer vec2 argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="vec">The vector</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Integer Vec2 Argument")
	bool SetIntVector2Arg(int32 index, const FIntPoint& vec);

	/// <summary>
	/// Sets a integer vec4 argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="vec">The vector</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Integer Vec4 Argument")
	bool SetIntVector4Arg(int32 index, const FIntVector4& vec);

	/// <summary>
	/// Sets a float vec2 argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="vec">The vector</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Float Vec2 Argument")
	bool SetVector2fArg(int32 index, const FVector2f& vec);

	/// <summary>
	/// Sets a float vec4 argument.
	/// </summary>
	/// <param name="index">The argument index</param>
	/// <param name="vec">The vector</param>
	/// <returns>True if the operation was successful</returns>
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Set Float Vec4 Argument")
	bool SetVector4fArg(int32 index, const FVector4f& vec);

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