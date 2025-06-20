#pragma once

#include "Core/CLCore.h"
#include "Core/CLContext.h"

#include "Core/ImageDefines.h"

#include <functional>

class UTexture2D;
class UTexture2DArray;
class UVolumeTexture;

namespace OpenCL
{
	class CommandQueue;

	class CLWORKS_API Image
	{
		friend CommandQueue;
	public:
		enum class Type : uint8_t
		{
			Undefined,

			Texture2D,
			Texture2DArray,
			Texture3D,

			COUNT
		};

		enum Format : uint32_t
		{
			Undefined = 0,

			// Data Type ----------------------------------
			UChar		= 1 << 0,
			UInt		= 1 << 1,
			SInt		= 1 << 2,
			HalfFloat	= 1 << 3,
			Float		= 1 << 4,
			// --------------------------------------------


			// Channels -----------------------------------
			R			= 1 << 5,
			RG			= 1 << 6,
			RGB			= 1 << 7,
			RGBA		= 1 << 8,
			// --------------------------------------------


			// Supported Combinations ---------------------
			R8			= R | UChar,
			RG8			= RG | UChar,
			RGBA8		= RGBA | UChar,

			R32U		= R | UInt,
			RG32U		= RG | UInt,
			RGBA32U		= RGBA | UInt,

			R32S		= R | SInt,

			R16F		= R | HalfFloat,
			RG16F		= RG | HalfFloat,
			RGBA16F		= RGBA | HalfFloat,

			R32F		= R | Float,
			RG32F		= RG | Float,
			RGBA32F		= RGBA | Float,
			// --------------------------------------------

			COUNT
		};
	public:
		Image();

		Image(const std::shared_ptr<OpenCL::Context>& context,
			  const std::shared_ptr<OpenCL::Device>& device,
			  uint32_t width,
			  uint32_t height,
			  uint32_t depthOrLayer = 1,
			  Format format = Format::RGBA8,
			  Type type = Type::Texture2D,
			  AccessType access = AccessType::READ_WRITE);
	public:
		cl_mem Get() const { return mpImage; }

		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }

		size_t GetPixelCount() const;
		uint8_t GetChannelCount() const;
		size_t GetChannelDataSize() const;
		size_t GetDataSize() const;

		TObjectPtr<UTexture2D> CreateUTexture2D(cl_command_queue queueOverride = nullptr,
												bool genMips = false,
												bool async = false,
												uint32_t maxBytesPerUpload = 64 * 2048);

		TObjectPtr<UTexture2DArray> CreateUTexture2DArray(cl_command_queue queueOverride = nullptr,
														  bool genMips = false);

		TObjectPtr<UVolumeTexture> CreateUVolumeTexture(cl_command_queue queueOverride = nullptr);

		bool UploadToUTexture2D(TObjectPtr<UTexture2D> output,
								cl_command_queue queueOverride = nullptr,
								bool genMips = false,
								bool async = false,
								uint32_t maxBytesPerUpload = 64 * 2048);
		
		bool UploadToUTextureRenderTarget2D(TObjectPtr<UTextureRenderTarget2D> output,
											cl_command_queue queueOverride = nullptr,
											bool genMips = false,
											const std::function<void()>& onUploadComplete = nullptr);

		bool UploadToUTexture2DArray(TObjectPtr<UTexture2DArray> output,
									 cl_command_queue queueOverride = nullptr,
									 bool genMips = false);

		bool UploadToUVolumeTexture(TObjectPtr<UVolumeTexture> output,
									cl_command_queue queueOverride = nullptr);
	private:
		cl_mem CreateCLImage();

		bool ReadFromCL(void** output = nullptr,
						cl_command_queue overrideQueue = nullptr,
						bool isBlocking = true) const;

		bool GenerateMips2D(std::vector<Mip>& output,
							void* src);

		void WriteToUTexture2D(TObjectPtr<UTexture2D> texture, 
							   void* src,
							   bool genMips);

		void WriteToUTexture2D_Async(TObjectPtr<UTexture2D> texture, 
									 void* src,
									 bool genMips,
									 uint32_t maxBytesPerUpload,
									 const std::function<void()>& onUploadComplete = nullptr);

		void WriteToUTexture2DArray(TObjectPtr<UTexture2DArray> texture, 
									void* src,
									bool genMips);
	private:
		cl_mem mpImage = nullptr;

		std::weak_ptr<OpenCL::Context> mpContext;
		std::weak_ptr<OpenCL::Device> mpDevice;

		Format mFormat = Format::Undefined;
		Type mType = Type::Undefined;
		AccessType mAccess = AccessType::READ_WRITE;

		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		uint32_t mDepthOrLayer = 0;
	};

	namespace Utils
	{
		static EPixelFormat FormatToPixelFormat(Image::Format format)
		{
			EPixelFormat pixelFormat = PF_Unknown;
			switch (format)
			{
				case Image::Format::R8:
					return PF_R8;
				case Image::Format::RG8:
					return PF_R8G8;
				case Image::Format::RGBA8:
					return PF_R8G8B8A8;

				case Image::Format::R32U:
					return PF_R32_UINT;
				case Image::Format::RG32U:
					return PF_R32G32_UINT;
				case Image::Format::RGBA32U:
					return PF_R32G32B32A32_UINT;

				case Image::Format::R32S:
					return PF_R32_SINT;

				case Image::Format::R16F:
					return PF_R16F;
				case Image::Format::RG16F:
					return PF_G16R16F;
				case Image::Format::RGBA16F:
					return PF_FloatRGBA;

				case Image::Format::R32F:
					return PF_R32_FLOAT;
				case Image::Format::RG32F:
					return PF_G32R32F;
				case Image::Format::RGBA32F:
					return PF_A32B32G32R32F;
				default:
					return PF_Unknown;
			}
		}
	}
}