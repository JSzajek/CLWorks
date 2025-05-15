#include "CLWorksLibrary.h"

#include "CLWorksLib.h"

#include <memory>

DEFINE_LOG_CATEGORY(LogCLWorksBlueprint);

#define LOCTEXT_NAMESPACE "FCLWorksBlueprintModule"

inline static UCLContextObject* mpGlobalContext = nullptr;

void FCLWorksBlueprintModule::StartupModule()
{
	UCLWorksLibrary::InitializeLibray();
}

void FCLWorksBlueprintModule::ShutdownModule()
{
	UCLWorksLibrary::DeinitializeLibray();
}

void UCLWorksLibrary::InitializeLibray()
{
	mpGlobalContext = UCLWorksLibrary::CreateCustomContext(0);
}

void UCLWorksLibrary::DeinitializeLibray()
{
	if (mpGlobalContext)
	{
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
												  UCLContextObject* contextOverride)
{
	UCLBufferObject* buffer = NewObject<UCLBufferObject>(GetTransientPackage(), NAME_None, RF_Transient);

	buffer->Initialize(contextOverride ? contextOverride : mpGlobalContext,
					   (void*)values.GetData(),
					   values.Num() * sizeof(int32),
					   access);

	if (!buffer->GetData())
	{
		buffer->ConditionalBeginDestroy();
		return nullptr;
	}
	return buffer;
}

UCLBufferObject* UCLWorksLibrary::CreateFloatBuffer(const TArray<float>& values, 
													UCLAccessType access,
													UCLContextObject* contextOverride)
{
	UCLBufferObject* buffer = NewObject<UCLBufferObject>(GetTransientPackage(), NAME_None, RF_Transient);

	buffer->Initialize(contextOverride ? contextOverride : mpGlobalContext,
					   (void*)values.GetData(),
					   values.Num() * sizeof(float),
					   access);

	if (!buffer->GetData())
	{
		buffer->ConditionalBeginDestroy();
		return nullptr;
	}
	return buffer;
}

TArray<int32> UCLWorksLibrary::ReadIntBuffer(UCLBufferObject* buffer,
											 int32 numElements,
											 UCLCommandQueueObject* queue,
											 UCLContextObject* contextOverride)
{
	if (!queue->IsValid())
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Invalid Command Queue!"));
		return {};
	}

	TArray<int32> output;
	output.SetNumZeroed(numElements);

	queue->mpQueue->ReadBuffer(*buffer->mpBuffer, 
							   numElements * sizeof(int32),
							   output.GetData());

	return output;
}

TArray<float> UCLWorksLibrary::ReadFloatBuffer(UCLBufferObject* buffer, 
											   int32 numElements,
											   UCLCommandQueueObject* queue,
											   UCLContextObject* contextOverride)
{
	if (!queue->IsValid())
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Invalid Command Queue!"));
		return {};
	}

	TArray<float> output;
	output.SetNumZeroed(numElements);

	queue->mpQueue->ReadBuffer(*buffer->mpBuffer, 
							   numElements * sizeof(float),
							   output.GetData());

	return output;
}

UCLImageObject* UCLWorksLibrary::CreateImage(int32 width, 
											 int32 height, 
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
			break;
		case UCLImageType::Texture3D:
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
								 UCLCommandQueueObject* queue, 
								 int64 dimensions, 
								 const TArray<int64>& workCount)
{
	if (workCount.Num() != dimensions)
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Mismatched Dimensions and Work Size: %d vs %d"), dimensions, workCount.Num());
		return false;
	}

	if (!queue->IsValid())
	{
		UE_LOG(LogCLWorksBlueprint, Warning, TEXT("Invalid Command Queue!"));
		return false;
	}

	queue->mpQueue->EnqueueRange(*program->mpKernel, dimensions, (size_t*)workCount.GetData());

	return true;
}
