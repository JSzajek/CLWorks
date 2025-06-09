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
	class CLWORKS_API Image
	{
		friend class CommandQueue;
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
			HalfFloat	= 1 << 1,
			Float		= 1 << 2,
			// --------------------------------------------


			// Channels -----------------------------------
			R			= 1 << 3,
			RGBA		= 1 << 4,
			// --------------------------------------------


			// Short-hand Combinations --------------------
			R8			= R | UChar,
			RGBA8		= RGBA |UChar,

			R16F		= R | HalfFloat,
			RGBA16F		= RGBA | HalfFloat,

			R32F		= R | Float,
			RGBA32F		= RGBA | Float,
			// --------------------------------------------

			COUNT
		};
	public:
		Image();

		Image(const OpenCL::Context& context,
			  const OpenCL::Device& device,
			  uint32_t width,
			  uint32_t height,
			  uint32_t depthOrLayer = 1,
			  Format format = Format::RGBA8,
			  Type type = Type::Texture2D,
			  AccessType access = AccessType::READ_WRITE);

		Image(cl_context context,
			  cl_device_id device,
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
											bool genMips = false);

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

		void WriteToUTextureRenderTarget2D(TObjectPtr<UTextureRenderTarget2D> texture,
										   void* src);

		void WriteToUTextureRenderTarget2D_Async(TObjectPtr<UTextureRenderTarget2D> texture,
												 void* src);

		void WriteToUTexture2DArray(TObjectPtr<UTexture2DArray> texture, 
									void* src,
									bool genMips);
	private:
		cl_context mpContext = nullptr;
		cl_device_id mpDevice = nullptr;
		cl_mem mpImage = nullptr;

		Format mFormat = Format::Undefined;
		Type mType = Type::Undefined;
		AccessType mAccess = AccessType::READ_WRITE;

		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		uint32_t mDepthOrLayer = 0;
	};
}