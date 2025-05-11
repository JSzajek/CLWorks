#pragma once

#include "Core/CLCore.h"
#include "Core/CLContext.h"

class UTexture2D;
class UTexture2DArray;
class UVolumeTexture;

namespace OpenCL
{
	class Texture
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

			// Data Type
			UChar		= 1 << 0,
			HalfFloat	= 1 << 1,
			Float		= 1 << 2,

			// Channels
			R			= 1 << 3,
			RGBA		= 1 << 4,

			R8			= R | UChar,
			RGBA8		= RGBA |UChar,

			R16F		= R | HalfFloat,
			RGBA16F		= RGBA | HalfFloat,

			R32F		= R | Float,
			RGBA32F		= RGBA | Float,

			COUNT
		};
	public:
		Texture(const OpenCL::Context& context,
				const OpenCL::Device& device,
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
		size_t GetChannelCount() const;

		TObjectPtr<UTexture2D> CreateUTexture2D();
		bool UploadToUTexture2D(TObjectPtr<UTexture2D> texture);

		TObjectPtr<UTexture2DArray> CreateUTexture2DArray();
		bool UploadToUTexture2DArray(TObjectPtr<UTexture2DArray> texture);

		TObjectPtr<UVolumeTexture> CreateUVolumeTexture();
		bool UploadToUVolumeTexture(TObjectPtr<UVolumeTexture> texture);
	private:
		cl_mem CreateCLImage();

		bool ReadFromCL(void** output = nullptr,
						cl_command_queue overrideQueue = nullptr,
						bool isBlocking = true) const;

		bool WriteToUTexture(TObjectPtr<UTexture> texture);
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