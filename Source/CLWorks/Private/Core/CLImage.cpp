#include "Core/CLImage.h"

#include "CLWorksLog.h"

#include "Core/CLCommandQueue.h"

#include "Utils/MipGenerator.h"

#include "Engine/Texture2D.h"
#include "Engine/Texture2DArray.h"
#include "Engine/VolumeTexture.h"

#include "RHICommandList.h"

namespace OpenCL
{
	Image::Image()
		: mpContext(nullptr),
		mpDevice(nullptr),
		mpImage(nullptr)
	{
	}

	Image::Image(const OpenCL::Context& context, 
				 const OpenCL::Device& device,
				 uint32_t width,
				 uint32_t height,
				 uint32_t depthOrLayer,
				 Format format,
				 Type type,
				 AccessType access)
		: mpContext(context.Get()),
		mpDevice(device.Get()),
		mpImage(nullptr),
		mFormat(format),
		mType(type),
		mAccess(access),
		mWidth(width),
		mHeight(height),
		mDepthOrLayer(depthOrLayer)
	{
		if (device.AreImagesSupported())
			mpImage = CreateCLImage();
	}

	Image::Image(cl_context context, 
				 cl_device_id device, 
				 uint32_t width, 
				 uint32_t height, 
				 uint32_t depthOrLayer, 
				 Format format, 
				 Type type, 
				 AccessType access)
		: mpContext(context),
		mpDevice(device),
		mpImage(nullptr),
		mFormat(format),
		mType(type),
		mAccess(access),
		mWidth(width),
		mHeight(height),
		mDepthOrLayer(depthOrLayer)
	{
		mpImage = CreateCLImage();
	}

	size_t Image::GetPixelCount() const
	{
		return mWidth * mHeight * mDepthOrLayer;
	}

	uint8_t Image::GetChannelCount() const
	{
		if ((mFormat & Format::R) > 0)
			return 1;
		else if ((mFormat & Format::RGBA) > 0)
			return 4;
		return 0;
	}

	size_t Image::GetChannelDataSize() const
	{
		if ((mFormat & Format::UChar) > 0)
			return sizeof(uint8_t);
		else if ((mFormat & Format::HalfFloat) > 0)
			return sizeof(FFloat16);
		else if ((mFormat & Format::Float) > 0)
			return sizeof(float);
		return 0;
	}

	size_t Image::GetDataSize() const
	{
		if ((mFormat & Format::UChar) > 0)
			return GetPixelCount() * GetChannelCount() * sizeof(uint8_t);
		else if ((mFormat & Format::HalfFloat) > 0)
			return GetPixelCount() * GetChannelCount() * sizeof(FFloat16);
		else if ((mFormat & Format::Float) > 0)
			return GetPixelCount() * GetChannelCount() * sizeof(float);
		return 0;
	}

	TObjectPtr<UTexture2D> Image::CreateUTexture2D(const OpenCL::CommandQueue& queue,
												   bool genMips)
	{
		return CreateUTexture2D(queue.Get(), genMips);
	}

	TObjectPtr<UTexture2D> Image::CreateUTexture2D(cl_command_queue queueOverride,
												   bool genMips)
	{
		if (!mpImage)
			return nullptr;

		EPixelFormat pixelFormat = PF_Unknown;
		switch (mFormat)
		{
			case Format::R8:
				pixelFormat = PF_R8;
				break;
			case Format::RGBA8:
				pixelFormat = PF_R8G8B8A8;
				break;
			case Format::R16F:
				pixelFormat = PF_R16F;
				break;
			case Format::RGBA16F:
				pixelFormat = PF_FloatRGBA;
				break;
			case Format::R32F:
				pixelFormat = PF_R32_FLOAT;
				break;
			case Format::RGBA32F:
				pixelFormat = PF_A32B32G32R32F;
				break;
			default:
				UE_LOG(LogCLWorks, Warning, TEXT("Creation CL Image Invalid Texture Format: %d!"), mFormat);
				return nullptr;
		}


		void* pixelData = nullptr;
		if (!ReadFromCL(&pixelData, queueOverride))
			return nullptr;

		const size_t dataSize = GetDataSize();

		//UTexture2D* texture = UTexture2D::CreateTransient(mWidth, mHeight, pixelFormat);

		UTexture2D* texture = NewObject<UTexture2D>(GetTransientPackage(),
												    NAME_None,
												    RF_Transient);

		texture->SetPlatformData(new FTexturePlatformData());
		texture->GetPlatformData()->SizeX = mWidth;
		texture->GetPlatformData()->SizeY = mHeight;
		texture->GetPlatformData()->SetNumSlices(1);
		texture->GetPlatformData()->PixelFormat = pixelFormat;
		
		WriteToUTexture2D(texture, pixelData, genMips);

		// Trigger render resource update
		//output->UpdateResource();
		
		return texture;
	}

	bool Image::UploadToUTexture2D(TObjectPtr<UTexture2D> texture, 
								   const OpenCL::CommandQueue& queue,
								   bool genMips)
	{
		return UploadToUTexture2D(texture, queue.Get(), genMips);
	}

	bool Image::UploadToUTexture2D(TObjectPtr<UTexture2D> output,
								   cl_command_queue queueOverride,
								   bool genMips)
	{
		const size_t output_width = output->GetSizeX();
		const size_t output_height = output->GetSizeY();
		if (output_width != mWidth || output_height != mHeight)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatched Texture with Size: %d x %d to Output Size: %d x %d!"), mWidth, mHeight, output_width, output_height);
			return false;
		}

		const size_t internal_dataSize = GetDataSize();
		const size_t output_dataSize = output_width * output_height * GPixelFormats[output->GetPixelFormat()].BlockBytes;
		if (internal_dataSize != output_dataSize)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatched Texture Format - Input Data Size: %d to Output Data Size: %d!"), internal_dataSize, output_dataSize);
			return false;
		}

		void* pixelData = nullptr;
		if (!ReadFromCL(&pixelData, queueOverride))
			return false;

		WriteToUTexture2D(output, pixelData, genMips);

		// Trigger render resource update
		//output->UpdateResource();

		return true;
	}

	TObjectPtr<UTexture2DArray> Image::CreateUTexture2DArray()
	{
		// TODO:: Implement
		return nullptr;
	}

	bool Image::UploadToUTexture2DArray(TObjectPtr<UTexture2DArray> texture)
	{
		// TODO:: Implement
		return false;
	}

	TObjectPtr<UVolumeTexture> Image::CreateUVolumeTexture()
	{
		// TODO:: Implement
		return nullptr;
	}

	bool Image::UploadToUVolumeTexture(TObjectPtr<UVolumeTexture> texture)
	{
		// TODO:: Implement
		return false;
	}

	cl_mem Image::CreateCLImage()
	{
		cl_image_desc desc = {};
		desc.image_width = mWidth;
		desc.image_height = mHeight;
		switch (mType)
		{
			case Type::Texture2D:
			{
				desc.image_type = CL_MEM_OBJECT_IMAGE2D;
				break;
			}
			case Type::Texture2DArray:
			{
				desc.image_type = CL_MEM_OBJECT_IMAGE2D_ARRAY;
				desc.image_array_size = mDepthOrLayer;
				break;
			}
			case Type::Texture3D:
			{
				desc.image_type = CL_MEM_OBJECT_IMAGE3D;
				desc.image_depth = mDepthOrLayer;
				break;
			}
			default:
			{
				UE_LOG(LogCLWorks, Warning, TEXT("Creation CL Image Invalid Texture Type: %d!"), mType);
				return nullptr;
			}
		}

		cl_image_format format = {};
		if ((mFormat & Format::UChar) > 0)
		{
			format.image_channel_data_type = CL_UNORM_INT8;
		}
		else if ((mFormat & Format::HalfFloat) > 0)
		{
			format.image_channel_data_type = CL_HALF_FLOAT;
		}
		else if ((mFormat & Format::Float) > 0)
		{
			format.image_channel_data_type = CL_FLOAT;
		}
		else
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Creation CL Image Invalid Image Format Data Type: %d!"), mFormat);
			return nullptr;
		}

		if ((mFormat & Format::R) > 0)
		{
			format.image_channel_order = CL_R;
		}
		else if ((mFormat & Format::RGBA) > 0)
		{
			format.image_channel_order = CL_RGBA;
		}
		else
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Creation CL Image Invalid Image Format Channel: %d!"), mFormat);
			return nullptr;
		}

		int32_t access = 0;
		switch (mAccess)
		{
		case AccessType::READ_ONLY:
			access = CL_MEM_READ_ONLY;
			break;
		case AccessType::WRITE_ONLY:
			access = CL_MEM_READ_WRITE;
			break;
		case AccessType::READ_WRITE:
			access = CL_MEM_READ_WRITE;
			break;
		default:
			UE_LOG(LogCLWorks, Warning, TEXT("Creation CL Image Invalid Access Type: %d!"), mAccess);
			return nullptr;
		}

		int32_t err = 0;
		cl_mem img = clCreateImage(mpContext, access, &format, &desc, nullptr, &err);
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Failed CL Image Creation: %d"), err);
			return nullptr;
		}
		return img;
	}

	bool Image::ReadFromCL(void** output,
						   cl_command_queue overrideQueue,
						   bool isBlocking) const
	{
		const size_t origin[3] = { 0, 0, 0 };
		const size_t region[3] = { mWidth, mHeight, mDepthOrLayer };

		size_t pixelCount	= GetPixelCount();
		size_t channelCount = GetChannelCount();

		void* data = nullptr;
		if (*output == nullptr)
		{
			if ((mFormat & Format::UChar) > 0)
			{
				data = new uint8_t[pixelCount * channelCount];
			}
			else if ((mFormat & Format::HalfFloat) > 0)
			{
				data = new FFloat16[pixelCount * channelCount];
			}
			else if ((mFormat & Format::Float) > 0)
			{
				data = new float[pixelCount * channelCount];
			}
			else
			{
				UE_LOG(LogCLWorks, Warning, TEXT("Invalid Image Data Format"));
				return false;
			}

			*output = data;
		}
		else
		{
			data = *output;
		}

		int32_t err = 0;
		if (overrideQueue)
		{
			err = clEnqueueReadImage(overrideQueue,
									 mpImage, 
									 isBlocking ? CL_TRUE : CL_FALSE,
									 origin, 
									 region, 
									 0, 
									 0, 
									 data,
									 0, 
									 nullptr, 
									 nullptr);
		}
		else
		{
			OpenCL::CommandQueue queue(mpContext, mpDevice);
			err = clEnqueueReadImage(queue.Get(),
									 mpImage, 
									 isBlocking ? CL_TRUE : CL_FALSE,
									 origin, 
									 region, 
									 0,
									 0,
									 data,
									 0,
									 nullptr, 
									 nullptr);
		}

		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Failed Reading Image: %d"), err);
			return false;
		}
		return true;
	}

	bool Image::GenerateMips(std::vector<Mip>& output, 
							 void* src)
	{
		const uint8_t channelCount = GetChannelCount();

		if ((mFormat & Format::UChar) > 0)
			MipGenerator::GenerateMipsInt8(output, (uint8_t*)src, mWidth, mHeight, channelCount);
		else if ((mFormat & Format::HalfFloat) > 0)
			MipGenerator::GenerateMipsFloat16(output, (FFloat16*)src, mWidth, mHeight, channelCount);
		else if ((mFormat & Format::Float) > 0)
			MipGenerator::GenerateMipsFloat(output, (float*)src, mWidth, mHeight, channelCount);
		else
			return false;
		return true;
	}

	void Image::WriteToUTexture2D(TObjectPtr<UTexture2D> texture, 
								  void* src, 
								  bool genMips)
	{
		const size_t channelDataSize = GetChannelDataSize();
		const uint8_t channelCount = GetChannelCount();

		std::vector<Mip> mips;
		if (genMips)
		{
			GenerateMips(mips, src);
		}
		else
		{
			mips.emplace_back(Mip{ mWidth, mHeight, channelCount, src });
		}

		FTexturePlatformData* platformData = texture->GetPlatformData();
		if (mips.size() != platformData->Mips.Num())
		{
			// Resize Mips to Match
			if (platformData->Mips.Num() < mips.size())
			{
				for (uint32_t i = platformData->Mips.Num(); i < mips.size(); ++i)
					platformData->Mips.Add(new FTexture2DMipMap());
			}
			else
			{
				platformData->Mips.Reset(mips.size());
			}
		}

		for (uint32_t i = 0; i < mips.size(); ++i)
		{
			Mip& dataMip = mips[i];
			const uint32_t mipDataSize = dataMip.mWidth * dataMip.mHeight * channelCount * channelDataSize;

			FTexture2DMipMap* mip = &platformData->Mips[i];

			mip->SizeX = dataMip.mWidth;
			mip->SizeY = dataMip.mHeight;
			mip->SizeZ = 1;

			mip->BulkData.Lock(LOCK_READ_WRITE);
			void* DestImageData = mip->BulkData.Realloc(mipDataSize);
			FMemory::Memcpy(DestImageData, src, mipDataSize);
			mip->BulkData.Unlock();

			// Clean up memory
			delete[] dataMip.mPixels;
			dataMip.mPixels = nullptr;
		}

		texture->UpdateResource();
	}

	void Image::WriteToUTexture2D_Async(TObjectPtr<UTexture2D> texture, 
										void* src, 
										bool genMips)
	{

	}
}