#pragma once

#include "Modules/ModuleManager.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/CoreDelegates.h"

#include "Objects/CLObjectDefines.h"

#include "CLWorksLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCLWorksBlueprint, Log, All);

class FCLWorksBlueprintModule final : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
};


// Forward Declaration --------------------------
class UCLBufferObject;
class UCLContextObject;
class UCLCommandQueueObject;
class UCLImageObject;
class UCLProgramObject;
class UCLProgramAsset;

class UTexture2D;
class UTextureRenderTarget2D;
// ----------------------------------------------

UCLASS(MinimalAPI)
class  UCLWorksLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static void InitializeLibray();
	static void DeinitializeLibray();
public:
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Create Custom Context")
	static UCLContextObject* CreateCustomContext(int32 deviceIndex);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Create Command Queue")
	static UCLCommandQueueObject* CreateCommandQueue(UCLContextObject* contextOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Create Program")
	static UCLProgramObject* CreateProgram(UCLProgramAsset* program, 
										   const FString& kernelName,
										   UCLContextObject* contextOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Create Integer Buffer")
	static UCLBufferObject* CreateIntBuffer(const TArray<int32>& buffer,
											UCLAccessType access = UCLAccessType::READ_WRITE,
											UCLContextObject* contextOverride = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Create Float Buffer")
	static UCLBufferObject* CreateFloatBuffer(const TArray<float>& buffer,
											  UCLAccessType access = UCLAccessType::READ_WRITE,
											  UCLContextObject* contextOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Create Image")
	static UCLImageObject* CreateImage(int32 width, 
									   int32 height,
									   UCLImageType type = UCLImageType::Texture2D,
									   UCLImageFormat format = UCLImageFormat::RGBA8,
									   UCLAccessType access = UCLAccessType::READ_WRITE,
									   UCLContextObject* contextOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Run Program")
	static bool RunProgram(UCLProgramObject* program,
						   int64 dimensions,
						   const TArray<int64>& workCount,
						   UCLCommandQueueObject* queue = nullptr);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Read Integer Buffer")
	static TArray<int32> ReadIntBuffer(UCLBufferObject* buffer,
									   int32 numElements,
									   UCLCommandQueueObject* queue = nullptr,
									   UCLContextObject* contextOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Read Float Buffer")
	static TArray<float> ReadFloatBuffer(UCLBufferObject* buffer,
										 int32 numElements,
										 UCLCommandQueueObject* queue = nullptr,
										 UCLContextObject* contextOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Convert To Texture2D")
	static UTexture2D* ImageToTexture2D(UCLImageObject* image,
									    UCLCommandQueueObject* queue = nullptr,
									    bool generateMipMaps = false);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Convert To Texture2DArray")
	static UTexture2DArray* ImageToTexture2DArray(UCLImageObject* image,
												  UCLCommandQueueObject* queue = nullptr,
												  bool generateMipMaps = false);

	UFUNCTION(BlueprintCallable, Category = "OpenCL", DisplayName = "Write To UTextureRenderTarget2D")
	static bool ImageToRenderTarget2D(UTextureRenderTarget2D* output,
									  UCLImageObject* image,
									  UCLCommandQueueObject* queue = nullptr);
};