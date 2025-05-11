#include "Core/CLTexture.h"

#include "Core/CLCommandQueue.h"

#include "CLWorksLog.h"

namespace OpenCL
{

	Texture::Texture(const OpenCL::Context& context, 
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

	size_t Texture::GetPixelCount() const
	{
		return mWidth * mHeight * mDepthOrLayer;
	}

	size_t Texture::GetChannelCount() const
	{
		if ((mFormat & Format::R) > 0)
		{
			return 1;
		}
		else if ((mFormat & Format::RGBA) > 0)
		{
			return 4;
		}
		return 0;
	}

	TObjectPtr<UTexture2D> Texture::CreateUTexture2D()
	{
		void* data = nullptr;
		ReadFromCL(&data);

		// TODO:: Implement Texture Creation and Upload

		return nullptr;
	}

	bool Texture::UploadToUTexture2D(TObjectPtr<UTexture2D> texture)
	{
		// TODO:: Implement
		return false;
	}

	TObjectPtr<UTexture2DArray> Texture::CreateUTexture2DArray()
	{
		// TODO:: Implement
		return nullptr;
	}

	bool Texture::UploadToUTexture2DArray(TObjectPtr<UTexture2DArray> texture)
	{
		// TODO:: Implement
		return false;
	}

	TObjectPtr<UVolumeTexture> Texture::CreateUVolumeTexture()
	{
		// TODO:: Implement
		return nullptr;
	}

	bool Texture::UploadToUVolumeTexture(TObjectPtr<UVolumeTexture> texture)
	{
		// TODO:: Implement
		return false;
	}

	cl_mem Texture::CreateCLImage()
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

	bool Texture::ReadFromCL(void** output,
						     cl_command_queue overrideQueue,
						     bool isBlocking) const
	{
		const size_t origin[3] = { 0, 0, 0 };
		const size_t region[3] = { mWidth, mHeight, mDepthOrLayer };

		size_t pixelCount	= GetPixelCount();
		size_t channelCount = GetChannelCount();

		void* data = nullptr;
		if (output == nullptr)
		{
			if ((mFormat & Format::UChar) > 0)
			{
				data = new uint8_t[pixelCount * channelCount];
			}
			else if ((mFormat & Format::HalfFloat) > 0)
			{
				// TODO:: Implement
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
									 256 * channelCount, 
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

	bool Texture::WriteToUTexture(TObjectPtr<UTexture> texture)
	{
		
		return false;
	}
}