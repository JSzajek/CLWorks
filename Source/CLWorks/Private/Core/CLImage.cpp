#include "Core/CLImage.h"

#include "CLWorksLog.h"

#include "Core/CLCommandQueue.h"

#include "Utils/MipGenerator.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"
#include "Engine/VolumeTexture.h"

#include "RHICommandList.h"
#include "Render/UTextureUtils.h"
#include "TextureResource.h"

namespace OpenCL
{
	Image::Image()
		: mpImage(nullptr),
		mpContext(),
		mpDevice()
	{
	}

	Image::Image(const std::shared_ptr<OpenCL::Context>& context, 
				 const std::shared_ptr<OpenCL::Device>& device,
				 uint32_t width,
				 uint32_t height,
				 uint32_t depthOrLayer,
				 Format format,
				 Type type,
				 AccessType access)
		: mpImage(nullptr),
		mpContext(context),
		mpDevice(device),
		mFormat(format),
		mType(type),
		mAccess(access),
		mWidth(width),
		mHeight(height),
		mDepthOrLayer(depthOrLayer)
	{
		if (device->AreImagesSupported())
		{
			if ((format & Format::HalfFloat) > 0 && !device->IsExtensionSupported("cl_khr_fp16"))
				return;

			mpImage = CreateCLImage();
		}
	}

	size_t Image::GetPixelCount() const
	{
		return mWidth * mHeight * mDepthOrLayer;
	}

	uint8_t Image::GetChannelCount() const
	{
		if ((mFormat & Format::R) > 0)
			return 1;
		else if ((mFormat & Format::RG) > 0)
			return 2;
		else if ((mFormat & Format::RGB) > 0)
			return 3;
		else if ((mFormat & Format::RGBA) > 0)
			return 4;
		return 0;
	}

	size_t Image::GetChannelDataSize() const
	{
		if ((mFormat & Format::UChar) > 0)
			return sizeof(uint8_t);
		else if ((mFormat & Format::UInt) > 0)
			return sizeof(uint32_t);
		else if ((mFormat & Format::SInt) > 0)
			return sizeof(int32_t);
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
												   bool isSRGB,
												   bool genMips,
												   bool async,
												   uint32_t maxBytesPerUpload)
	{
		if (mType != Type::Texture2D)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatching Image Type: %d"), mType);
			return nullptr;
		}

		if (!mpImage)
			return nullptr;

		EPixelFormat pixelFormat = Utils::FormatToPixelFormat(mFormat);
		if (pixelFormat == EPixelFormat::PF_Unknown)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Creation CL Image Invalid Texture Format: %d!"), mFormat);
			return nullptr;
		}


		void* pixelData = nullptr;
		if (!ReadFromCL(queue, &pixelData))
			return nullptr;

		const size_t dataSize = GetDataSize();

		UTexture2D* texture = NewObject<UTexture2D>(GetTransientPackage(),
												    NAME_None,
												    RF_Transient);
		texture->SRGB = isSRGB;

		FTexturePlatformData* platformData = new FTexturePlatformData();
		texture->SetPlatformData(platformData);
		platformData->SizeX = mWidth;
		platformData->SizeY = mHeight;
		platformData->SetNumSlices(1);
		platformData->PixelFormat = pixelFormat;
		
		if (async)
		{
			WriteToUTexture2D_Async(texture, pixelData, genMips, maxBytesPerUpload);
		}
		else
		{
			WriteToUTexture2D(texture, pixelData, genMips);
		}

		return texture;
	}

	TObjectPtr<UTexture2DArray> Image::CreateUTexture2DArray(const OpenCL::CommandQueue& queue,
															 bool isSRGB,
															 bool genMips)
	{
		if (!mpImage)
			return nullptr;

		if (mType != Type::Texture2DArray)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatching Image Type: %d"), mType);
			return nullptr;
		}

		EPixelFormat pixelFormat = Utils::FormatToPixelFormat(mFormat);
		if (pixelFormat == EPixelFormat::PF_Unknown)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Creation CL Image Invalid Texture Format: %d!"), mFormat);
			return nullptr;
		}


		void* pixelData = nullptr;
		if (!ReadFromCL(queue, &pixelData))
			return nullptr;

		const size_t dataSize = GetDataSize();

		UTexture2DArray* texture = NewObject<UTexture2DArray>(GetTransientPackage(),
															  NAME_None,
															  RF_Transient);
		texture->SRGB = isSRGB;

		FTexturePlatformData* platformData = new FTexturePlatformData();
		texture->SetPlatformData(platformData);
		platformData->SizeX = mWidth;
		platformData->SizeY = mHeight;
		platformData->SetNumSlices(mDepthOrLayer);
		platformData->PixelFormat = pixelFormat;

		WriteToUTexture2DArray(texture, pixelData, genMips);

		return texture;
	}

	TObjectPtr<UVolumeTexture> Image::CreateUVolumeTexture(const OpenCL::CommandQueue& queue)
	{
		throw std::exception("Not Implemented");
	}

	bool Image::UploadToUTexture2D(TObjectPtr<UTexture2D> output,
								   const OpenCL::CommandQueue& queue,
								   bool genMips,
								   bool async,
								   uint32_t maxBytesPerUpload)
	{
		if (mType != Type::Texture2D)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatching Image Type: %d"), mType);
			return false;
		}

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
		if (!ReadFromCL(queue, &pixelData))
			return false;

		if (async)
		{
			WriteToUTexture2D_Async(output, pixelData, genMips, maxBytesPerUpload);
		}
		else
		{
			WriteToUTexture2D(output, pixelData, genMips);
		}

		return true;
	}

	bool Image::UploadToUTextureRenderTarget2D(TObjectPtr<UTextureRenderTarget2D> output, 
											   const OpenCL::CommandQueue& queue, 
											   bool genMips,
											   const std::function<void()>& onUploadComplete)
	{
		if (mType != Type::Texture2D)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatching Image Type: %d"), mType);
			return false;
		}

		const size_t output_width = output->GetSurfaceWidth();
		const size_t output_height = output->GetSurfaceHeight();
		if (output_width != mWidth || output_height != mHeight)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatched Texture with Size: %d x %d to Output Size: %d x %d!"), mWidth, mHeight, output_width, output_height);
			return false;
		}

		const size_t internal_dataSize = GetDataSize();
		const size_t output_dataSize = output_width * output_height * GPixelFormats[output->GetFormat()].BlockBytes;
		if (internal_dataSize != output_dataSize)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatched Texture Format - Input Data Size: %d to Output Data Size: %d!"), internal_dataSize, output_dataSize);
			return false;
		}

		void* pixelData = nullptr;
		if (!ReadFromCL(queue, &pixelData))
			return false;


		// Create temporary Texture2D
		TObjectPtr<UTexture2D> texture = CreateUTexture2D(queue, genMips);


		// Blit Texture2D to RenderTarget2D
		UTextureUtils::BlitTextureToRenderTarget(texture, output, FIntPoint::ZeroValue, onUploadComplete);

		return true;
	}

	bool Image::UploadToUTexture2DArray(TObjectPtr<UTexture2DArray> output,
										const OpenCL::CommandQueue& queue, 
										bool genMips)
	{
		if (mType != Type::Texture2DArray)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatching Image Type: %d"), mType);
			return false;
		}

		const size_t output_width = output->GetSizeX();
		const size_t output_height = output->GetSizeY();
		const size_t output_slices = output->GetArraySize();
		if (output_slices != mDepthOrLayer)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatched Array Size: %d to Output Size: %d!"), mDepthOrLayer, output_slices);
			return false;
		}

		if (output_width != mWidth || output_height != mHeight)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatched Texture with Size: %d x %d to Output Size: %d x %d!"), mWidth, mHeight, output_width, output_height);
			return false;
		}

		const size_t internal_dataSize = GetDataSize();
		const size_t output_dataSize = output_width * output_height * output_slices * GPixelFormats[output->GetPixelFormat()].BlockBytes;
		if (internal_dataSize != output_dataSize)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Mismatched Texture Format - Input Data Size: %d to Output Data Size: %d!"), internal_dataSize, output_dataSize);
			return false;
		}

		void* pixelData = nullptr;
		if (!ReadFromCL(queue, &pixelData))
			return false;

		WriteToUTexture2DArray(output, pixelData, genMips);

		return true;
	}


	bool Image::UploadToUVolumeTexture(TObjectPtr<UVolumeTexture> output,
									   const OpenCL::CommandQueue& queue)
	{
		throw std::exception("Not Implemented");
	}

	bool Image::Fetch(const OpenCL::CommandQueue& queue, 
						 void* output, 
						 bool isBlocking) const
	{
		return ReadFromCL(queue, &output, isBlocking);
	}

	cl_mem Image::CreateCLImage()
	{
		const std::shared_ptr<Context> context_ptr = mpContext.lock();
		if (!context_ptr)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Invalid Program Context!"));
			return nullptr;
		}

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
		else if ((mFormat & Format::UInt) > 0)
		{
			format.image_channel_data_type = CL_UNSIGNED_INT32;
		}
		else if ((mFormat & Format::SInt) > 0)
		{
			format.image_channel_data_type = CL_SIGNED_INT32;
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
		else if ((mFormat & Format::RG) > 0)
		{
			format.image_channel_order = CL_RG;
		}
		else if ((mFormat & Format::RGB) > 0)
		{
			format.image_channel_order = CL_RGB;
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
		cl_mem img = clCreateImage(context_ptr->Get(), access, &format, &desc, nullptr, &err);
		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Failed CL Image Creation: %d"), err);
			return nullptr;
		}
		return img;
	}

	bool Image::ReadFromCL(const OpenCL::CommandQueue& queue,
						   void** output,
						   bool isBlocking) const
	{
		const std::shared_ptr<Context> context_ptr = mpContext.lock();
		if (!context_ptr)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Invalid Program Context!"));
			return false;
		}

		const std::shared_ptr<Device> device_ptr = mpDevice.lock();
		if (!device_ptr)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Invalid Program Device!"));
			return false;
		}

		const size_t origin[3] = { 0, 0, 0 };
		const size_t region[3] = { mWidth, mHeight, mDepthOrLayer };

		size_t pixelCount	= GetPixelCount();
		size_t channelCount = GetChannelCount();

		void* data = nullptr;
		if (*output == nullptr)
		{
			if ((mFormat & Format::UChar) > 0)
			{
				data = new uint8_t[pixelCount * channelCount * mDepthOrLayer];
			}
			else if ((mFormat & Format::UInt) > 0)
			{
				data = new uint32_t[pixelCount * channelCount * mDepthOrLayer];
			}
			else if ((mFormat & Format::SInt) > 0)
			{
				data = new int32_t[pixelCount * channelCount * mDepthOrLayer];
			}
			else if ((mFormat & Format::HalfFloat) > 0)
			{
				data = new FFloat16[pixelCount * channelCount * mDepthOrLayer];
			}
			else if ((mFormat & Format::Float) > 0)
			{
				data = new float[pixelCount * channelCount * mDepthOrLayer];
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
		if (queue.Get())
		{
			err = clEnqueueReadImage(queue,
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
			OpenCL::CommandQueue localqueue(context_ptr, device_ptr);
			err = clEnqueueReadImage(localqueue.Get(),
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

	bool Image::GenerateMips2D(std::vector<Mip>& output,
							   void* src)
	{
		const uint8_t channelCount = GetChannelCount();

		if ((mFormat & Format::UChar) > 0)
			MipGenerator::GenerateMipsInt8(output, static_cast<uint8_t*>(src), mWidth, mHeight, mDepthOrLayer, channelCount);
		else if ((mFormat & Format::UInt) > 0)
			MipGenerator::GenerateMipsUInt32(output, static_cast<uint32_t*>(src), mWidth, mHeight, mDepthOrLayer, channelCount);
		else if ((mFormat & Format::SInt) > 0)
			MipGenerator::GenerateMipsInt32(output, static_cast<int32_t*>(src), mWidth, mHeight, mDepthOrLayer, channelCount);
		else if ((mFormat & Format::HalfFloat) > 0)
			MipGenerator::GenerateMipsFloat16(output, static_cast<FFloat16*>(src), mWidth, mHeight, mDepthOrLayer, channelCount);
		else if ((mFormat & Format::Float) > 0)
			MipGenerator::GenerateMipsFloat(output, static_cast<float*>(src), mWidth, mHeight, mDepthOrLayer, channelCount);
		else
			return false;
		return true;
	}

	void Image::WriteToUTexture2D(TObjectPtr<UTexture2D> texture, 
								  void* src, 
								  bool genMips)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(Image::WriteToUTexture2D());

		const size_t channelDataSize = GetChannelDataSize();
		const uint8_t channelCount = GetChannelCount();

		std::vector<Mip> mips;
		if (genMips)
		{
			GenerateMips2D(mips, src);

			if (mips.empty())
			{
				UE_LOG(LogCLWorks, Warning, TEXT("Failed to Generate Mips!"));
				return;
			}
		}
		else
		{
			mips.emplace_back(Mip{ mWidth, 
								   mHeight, 
								   1, 
								   channelCount, 
								   src });
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
			FMemory::Memcpy(DestImageData, dataMip.mPixels, mipDataSize);
			mip->BulkData.Unlock();

			// Clean memory
			delete[] dataMip.mPixels;
			dataMip.mPixels = nullptr;
		}

		texture->UpdateResource();
	}

	void Image::WriteToUTexture2D_Async(TObjectPtr<UTexture2D> texture, 
										void* src,
										bool genMips,
										uint32_t maxBytesPerUpload,
										const std::function<void()>& onUploadComplete)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(Image::WriteToUTexture2D_Async());

		const size_t channelDataSize = GetChannelDataSize();
		const uint8_t channelCount = GetChannelCount();

		std::vector<Mip> mips;
		if (genMips)
		{
			GenerateMips2D(mips, src);
		}
		else
		{
			mips.emplace_back(Mip{ mWidth, mHeight, 1, channelCount, src });
		}

		FTexturePlatformData* platformData = texture->GetPlatformData();
		if (mips.size() != platformData->Mips.Num())
		{
			// Resize Mips to Match
			if (platformData->Mips.Num() < mips.size())
			{
				for (uint32_t i = platformData->Mips.Num(); i < mips.size(); ++i)
				{
					platformData->Mips.Add(new FTexture2DMipMap());
					FTexture2DMipMap* mip = &platformData->Mips[i];

					Mip& dataMip = mips[i];
					const uint32_t mipDataSize = dataMip.mWidth * dataMip.mHeight * channelCount * channelDataSize;

					mip->SizeX = dataMip.mWidth;
					mip->SizeY = dataMip.mHeight;
					mip->SizeZ = 1;

					mip->BulkData.Lock(LOCK_READ_WRITE);
					mip->BulkData.Realloc(mipDataSize);
					mip->BulkData.Unlock();
				}
			}
			else
			{
				platformData->Mips.Reset(mips.size());
			}

			texture->UpdateResource();
		}

		const int32 MipCount = mips.size();
		const int32 Width = mWidth;
		const int32 Height = mHeight;
		const int32 BytesPerPixel = channelDataSize * channelCount;

		std::atomic<size_t>* counter = new std::atomic<size_t>(0);

		for (size_t MipIndex = 0; MipIndex < MipCount; ++MipIndex)
		{
			Mip& mip = mips[MipIndex];
			const int32 RowPitch = mip.mWidth * BytesPerPixel;
			const int32 TotalBytes = RowPitch * mip.mHeight;

			void* SrcData = mip.mPixels;

			// Determine how many rows we can upload per batch
			const size_t RowsPerBatch = FMath::Max(1u, maxBytesPerUpload / RowPitch);
			const size_t NumBatches = FMath::DivideAndRoundUp(mip.mHeight, RowsPerBatch);

			FUpdateTextureRegion2D* regions =  new FUpdateTextureRegion2D[NumBatches];
			for (size_t BatchIndex = 0; BatchIndex < NumBatches; ++BatchIndex)
			{
				const size_t TargetY = BatchIndex * RowsPerBatch;
				const size_t BatchHeight = FMath::Min(RowsPerBatch, mip.mHeight - TargetY);

				FUpdateTextureRegion2D& region = regions[BatchIndex];

				region.DestX = 0;
				region.DestY = TargetY;
				region.SrcX = 0;
				region.SrcY = TargetY;
				region.Width = mip.mWidth;
				region.Height = BatchHeight;
			}

			texture->UpdateTextureRegions(MipIndex,
										  NumBatches,
										  regions,
										  RowPitch,
										  BytesPerPixel,
										  (uint8*)SrcData, [counter, MipCount, onUploadComplete](uint8* SrcData, const FUpdateTextureRegion2D* Regions)
										  {
												delete[] SrcData;
												delete[] Regions;

												(*counter)++;
												if (*counter == MipCount)
												{
													if (onUploadComplete)
														onUploadComplete();

													delete counter;
												}
										  });
		}
	}

	void Image::WriteToUTexture2DArray(TObjectPtr<UTexture2DArray> texture,
									   void* src, 
									   bool genMips)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(Image::WriteToUTexture2DArray());

		const size_t channelDataSize = GetChannelDataSize();
		const uint8_t channelCount = GetChannelCount();

		std::vector<Mip> mips;
		if (genMips)
		{
			GenerateMips2D(mips, src);
		}
		else
		{
			mips.emplace_back(Mip { mWidth, 
								    mHeight, 
								    mDepthOrLayer,
								    channelCount,
									src });
		}

		FTexturePlatformData* platformData = texture->GetPlatformData();
		if (mips.size() != platformData->Mips.Num())
		{
			// Resize Mips to Match
			if (platformData->Mips.Num() < mips.size())
			{
				for (int32_t i = platformData->Mips.Num(); i < mips.size(); ++i)
					platformData->Mips.Add(new FTexture2DMipMap());
			}
			else
			{
				platformData->Mips.Reset(mips.size());
			}
		}

		uint32_t texMipCnt = 0;
		for (Mip& dataMip : mips)
		{
			uint8_t* dataPix = (uint8_t*)dataMip.mPixels;

			const uint32_t mipDataSize = dataMip.mWidth * dataMip.mHeight * dataMip.mSlices * channelCount * channelDataSize;

			FTexture2DMipMap* mip = &platformData->Mips[texMipCnt];

			mip->SizeX = dataMip.mWidth;
			mip->SizeY = dataMip.mHeight;
			mip->SizeZ = dataMip.mSlices;

			mip->BulkData.Lock(LOCK_READ_WRITE);
			void* DestImageData = mip->BulkData.Realloc(mipDataSize);
			FMemory::Memcpy(DestImageData, dataMip.mPixels, mipDataSize);
			mip->BulkData.Unlock();

			++texMipCnt;

			// Clean memory
			delete[] dataMip.mPixels;
			dataMip.mPixels = nullptr;
		}

		texture->UpdateResource();
	}
}