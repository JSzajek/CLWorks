#include "CLWorksLibrary.h"

#include "CLWorksLib.h"

#include <memory>

DEFINE_LOG_CATEGORY(LogCLWorksBlueprint);

#define LOCTEXT_NAMESPACE "FCLWorksBlueprintModule"

UCLContextObject* UCLWorksLibrary::mpGlobalContext = nullptr;
UCLCommandQueueObject* UCLWorksLibrary::mpGlobalQueue = nullptr;

void FCLWorksBlueprintModule::StartupModule()
{
}

void FCLWorksBlueprintModule::ShutdownModule()
{
}

UCLWorksLibrary::UCLWorksLibrary(const class FObjectInitializer& ObjectInitializer)
{
	InitializeLibray();
}

void UCLWorksLibrary::BeginDestroy()
{
	Super::BeginDestroy();

	DeinitializeLibray();
}

void UCLWorksLibrary::InitializeLibray()
{
	mpGlobalContext = UCLWorksLibrary::CreateCustomContext(0);
	mpGlobalContext->AddToRoot();

	mpGlobalQueue = UCLWorksLibrary::CreateCommandQueue(mpGlobalContext);
	mpGlobalQueue->AddToRoot();
}

void UCLWorksLibrary::DeinitializeLibray()
{
	if (mpGlobalQueue)
	{
		mpGlobalQueue->RemoveFromRoot();
		mpGlobalQueue->ConditionalBeginDestroy();
		mpGlobalQueue = nullptr;
	}

	if (mpGlobalContext)
	{
		mpGlobalContext->RemoveFromRoot();
		mpGlobalContext->ConditionalBeginDestroy();
		mpGlobalContext = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCLWorksBlueprintModule, CLWorksBlueprint)



UCLContextObject* UCLWorksLibrary::CreateCustomContext(int32 deviceIndex)
{
	UCLContextObject* context = NewObject<UCLContextObject>(GetTransientPackage(), NAME_None, RF_Transient);
	context->Initialize(deviceIndex);

	if (!context->IsValid())
	{
		context->ConditionalBeginDestroy();
		return nullptr;
	}
	return context;
}

UCLCommandQueueObject* UCLWorksLibrary::CreateCommandQueue(UCLContextObject* contextOverride)
{
	UCLCommandQueueObject* queue = NewObject<UCLCommandQueueObject>(GetTransientPackage(), NAME_None, RF_Transient);

	queue->Initialize(contextOverride ? contextOverride : mpGlobalContext);

	if (!queue->IsValid())
	{
		queue->ConditionalBeginDestroy();
		return nullptr;
	}
	return queue;
}

UCLProgramObject* UCLWorksLibrary::CreateProgram(UCLProgramAsset* asset,
												 const FString& kernelName, 
												 UCLContextObject* contextOverride)
{
	if (!asset)
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Invalid Program Asset!"));
		return nullptr;
	}

	UCLProgramObject* program = NewObject<UCLProgramObject>(GetTransientPackage(), NAME_None, RF_Transient);
	
	program->Initialize(contextOverride ? contextOverride : mpGlobalContext, asset, kernelName);

	if (!program->IsValid())
	{
		program->ConditionalBeginDestroy();
		return nullptr;
	}
	return program;
}

UCLBufferObject* UCLWorksLibrary::CreateIntBuffer(const TArray<int32>& values,
												  UCLAccessType access,
												  UCLMemoryStrategy strategy,
												  UCLContextObject* contextOverride)
{
	UCLBufferObject* buffer = NewObject<UCLBufferObject>(GetTransientPackage(), NAME_None, RF_Transient);

	buffer->Initialize(contextOverride ? contextOverride : mpGlobalContext,
					   (void*)values.GetData(),
					   values.Num() * sizeof(int32),
					   access,
					   strategy);

	if (!buffer->GetData())
	{
		buffer->ConditionalBeginDestroy();
		return nullptr;
	}
	return buffer;
}

UCLBufferObject* UCLWorksLibrary::CreateFloatBuffer(const TArray<float>& values, 
													UCLAccessType access,
													UCLMemoryStrategy strategy,
													UCLContextObject* contextOverride)
{
	UCLBufferObject* buffer = NewObject<UCLBufferObject>(GetTransientPackage(), NAME_None, RF_Transient);

	buffer->Initialize(contextOverride ? contextOverride : mpGlobalContext,
					   (void*)values.GetData(),
					   values.Num() * sizeof(float),
					   access,
					   strategy);

	if (!buffer->GetData())
	{
		buffer->ConditionalBeginDestroy();
		return nullptr;
	}
	return buffer;
}

UCLBufferObject* UCLWorksLibrary::CreateIntVector2Buffer(const TArray<FIntPoint>& values,
														 UCLAccessType access, 
														 UCLMemoryStrategy strategy,
														 UCLContextObject* contextOverride)
{
	UCLBufferObject* buffer = NewObject<UCLBufferObject>(GetTransientPackage(), NAME_None, RF_Transient);

	buffer->Initialize(contextOverride ? contextOverride : mpGlobalContext,
					   (void*)values.GetData(),
					   values.Num() * sizeof(FIntPoint),
					   access,
					   strategy);

	if (!buffer->GetData())
	{
		buffer->ConditionalBeginDestroy();
		return nullptr;
	}
	return buffer;
}

UCLBufferObject* UCLWorksLibrary::CreateIntVector4Buffer(const TArray<FIntVector4>& values,
														 UCLAccessType access, 
														 UCLMemoryStrategy strategy,
														 UCLContextObject* contextOverride)
{
	UCLBufferObject* buffer = NewObject<UCLBufferObject>(GetTransientPackage(), NAME_None, RF_Transient);

	buffer->Initialize(contextOverride ? contextOverride : mpGlobalContext,
					   (void*)values.GetData(),
					   values.Num() * sizeof(FIntVector4),
					   access,
					   strategy);

	if (!buffer->GetData())
	{
		buffer->ConditionalBeginDestroy();
		return nullptr;
	}
	return buffer;
}

UCLBufferObject* UCLWorksLibrary::CreateVector2fBuffer(const TArray<FVector2f>& values,
													   UCLAccessType access, 
													   UCLMemoryStrategy strategy,
													   UCLContextObject* contextOverride)
{
	UCLBufferObject* buffer = NewObject<UCLBufferObject>(GetTransientPackage(), NAME_None, RF_Transient);

	buffer->Initialize(contextOverride ? contextOverride : mpGlobalContext,
					   (void*)values.GetData(),
					   values.Num() * sizeof(FVector2f),
					   access,
					   strategy);

	if (!buffer->GetData())
	{
		buffer->ConditionalBeginDestroy();
		return nullptr;
	}
	return buffer;
}

UCLBufferObject* UCLWorksLibrary::CreateVector4fBuffer(const TArray<FVector4f>& values,
													   UCLAccessType access, 
													   UCLMemoryStrategy strategy,
													   UCLContextObject* contextOverride)
{
	UCLBufferObject* buffer = NewObject<UCLBufferObject>(GetTransientPackage(), NAME_None, RF_Transient);

	buffer->Initialize(contextOverride ? contextOverride : mpGlobalContext,
					   (void*)values.GetData(),
					   values.Num() * sizeof(FVector4f),
					   access,
					   strategy);

	if (!buffer->GetData())
	{
		buffer->ConditionalBeginDestroy();
		return nullptr;
	}
	return buffer;
}

UCLImageObject* UCLWorksLibrary::CreateImage(int32 width,
											 int32 height,
											 int32 layers,
											 UCLImageType type, 
											 UCLImageFormat format, 
											 UCLAccessType access, 
											 UCLContextObject* contextOverride)
{
	UCLContextObject* context = contextOverride ? contextOverride : mpGlobalContext;

	if (!context->HasImageSupport())
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Failed Image Object Creation: No Image Support!"));
	}
	
	UCLImageObject* image = NewObject<UCLImageObject>(GetTransientPackage(), NAME_None, RF_Transient);

	switch (type)
	{
		case UCLImageType::Texture2D:
			image->Initialize2D(context,
								width,
								height,
								format,
								access);
			break;
		case UCLImageType::Texture2DArray:
			image->Initialize2DArray(context,
									 width,
									 height,
									 layers,
									 format,
									 access);
			break;
		case UCLImageType::Texture3D:
			throw std::exception("Not Implemented!");
			break;
	}

	if (!image->GetData())
	{
		image->ConditionalBeginDestroy();
		return nullptr;
	}
	return image;
}

bool UCLWorksLibrary::RunProgram(UCLProgramObject* program, 
								 int64 dimensions, 
								 const TArray<int64>& workCount,
								 UCLCommandQueueObject* queueOverride)
{
	if (workCount.Num() != dimensions)
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Mismatched Dimensions and Work Size: %d vs %d"), dimensions, workCount.Num());
		return false;
	}

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;

	commandQueue.EnqueueRange(*program->mpKernel, 
							  dimensions, 
							  (size_t*)workCount.GetData());

	return true;
}

TArray<int32> UCLWorksLibrary::ReadIntBuffer(UCLBufferObject* buffer,
											 int32 numElements,
											 UCLCommandQueueObject* queueOverride,
											 UCLContextObject* contextOverride)
{
	TArray<int32> output;
	output.SetNumZeroed(numElements);

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;
	buffer->mpBuffer->Fetch(commandQueue, output.GetData(), numElements * sizeof(int32), 0);

	return output;
}

TArray<float> UCLWorksLibrary::ReadFloatBuffer(UCLBufferObject* buffer, 
											   int32 numElements,
											   UCLCommandQueueObject* queueOverride,
											   UCLContextObject* contextOverride)
{
	TArray<float> output;
	output.SetNumZeroed(numElements);

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;
	buffer->mpBuffer->Fetch(commandQueue, output.GetData(), numElements * sizeof(float), 0);

	return output;
}

TArray<FIntPoint> UCLWorksLibrary::ReadIntVector2Buffer(UCLBufferObject* buffer, 
														int32 numElements, 
														UCLCommandQueueObject* queueOverride, 
														UCLContextObject* contextOverride)
{
	TArray<FIntPoint> output;
	output.SetNumZeroed(numElements);

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;
	buffer->mpBuffer->Fetch(commandQueue, output.GetData(), numElements * sizeof(FIntPoint), 0);

	return output;
}

TArray<FIntVector4> UCLWorksLibrary::ReadIntVector4Buffer(UCLBufferObject* buffer, 
														  int32 numElements, 
														  UCLCommandQueueObject* queueOverride, 
														  UCLContextObject* contextOverride)
{
	TArray<FIntVector4> output;
	output.SetNumZeroed(numElements);

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;
	buffer->mpBuffer->Fetch(commandQueue, output.GetData(), numElements * sizeof(FIntVector4), 0);

	return output;
}

TArray<FVector2f> UCLWorksLibrary::ReadVector2fBuffer(UCLBufferObject* buffer, 
													  int32 numElements, 
													  UCLCommandQueueObject* queueOverride, 
													  UCLContextObject* contextOverride)
{
	TArray<FVector2f> output;
	output.SetNumZeroed(numElements);

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;
	buffer->mpBuffer->Fetch(commandQueue, output.GetData(), numElements * sizeof(FVector2f), 0);

	return output;
}

TArray<FVector4f> UCLWorksLibrary::ReadVector4fBuffer(UCLBufferObject* buffer, 
													  int32 numElements, 
													  UCLCommandQueueObject* queueOverride, 
													  UCLContextObject* contextOverride)
{
	TArray<FVector4f> output;
	output.SetNumZeroed(numElements);

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;
	buffer->mpBuffer->Fetch(commandQueue, output.GetData(), numElements * sizeof(FVector4f), 0);

	return output;
}

UTexture2D* UCLWorksLibrary::ImageToTexture2D(UCLImageObject* image, 
											  UCLCommandQueueObject* queueOverride,
											  bool isSRGB,
											  bool generateMipMaps)
{
	if (image->GetData() == nullptr)
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Invalid Image!"));
		return nullptr;
	}

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;

	return image->mpImage->CreateUTexture2D(commandQueue, isSRGB, generateMipMaps);
}

UTexture2DArray* UCLWorksLibrary::ImageToTexture2DArray(UCLImageObject* image, 
														UCLCommandQueueObject* queueOverride, 
														bool isSRGB,
														bool generateMipMaps)
{
	if (image->GetData() == nullptr)
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Invalid Image!"));
		return nullptr;
	}

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;

	return image->mpImage->CreateUTexture2DArray(commandQueue, isSRGB, generateMipMaps);
}

bool UCLWorksLibrary::WriteToRenderTarget2D(UTextureRenderTarget2D* output,
											UCLImageObject* image, 
											UCLCommandQueueObject* queueOverride)
{
	if (image->GetData() == nullptr)
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Invalid Image!"));
		return false;
	}

	OpenCL::CommandQueue& commandQueue = queueOverride ? *queueOverride->mpQueue : *mpGlobalQueue->mpQueue;

	return image->mpImage->UploadToUTextureRenderTarget2D(output, commandQueue);
}