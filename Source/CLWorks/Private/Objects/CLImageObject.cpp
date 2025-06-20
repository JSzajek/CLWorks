#include "Objects/CLImageObject.h"

OpenCL::Image::Format FormatToCLFormat(UCLImageFormat format)
{
	switch (format)
	{
	case UCLImageFormat::R8:
		return OpenCL::Image::R8;
	case UCLImageFormat::RGBA8:
		return OpenCL::Image::RGBA8;
	case UCLImageFormat::R16:
		return OpenCL::Image::R16F;
	case UCLImageFormat::RGBA16:
		return OpenCL::Image::RGBA16F;
	case UCLImageFormat::R32:
		return OpenCL::Image::R32F;
	case UCLImageFormat::RGBA32:
		return OpenCL::Image::RGBA32F;
	default:
		return OpenCL::Image::Undefined;
	}
}

OpenCL::AccessType AccessToCLAccess(UCLAccessType access)
{
	switch (access)
	{
	case UCLAccessType::READ_ONLY:
		return OpenCL::AccessType::READ_ONLY;
	case UCLAccessType::WRITE_ONLY:
		return OpenCL::AccessType::WRITE_ONLY;
	case UCLAccessType::READ_WRITE:
		return OpenCL::AccessType::READ_WRITE;
	default:
		return OpenCL::AccessType::COUNT;
	}
}

void UCLImageObject::Initialize2D(const TObjectPtr<UCLContextObject>& context, 
								  int32_t width, 
								  int32_t height, 
								  UCLImageFormat format, 
								  UCLAccessType type)
{
	Width = width;
	Height = height;
	Depth = 1;
	Type = UCLImageType::Texture2D;

	OpenCL::AccessType access = AccessToCLAccess(type);
	OpenCL::Image::Format imgFormat = FormatToCLFormat(format);

	mpImage = std::make_shared<OpenCL::Image>(context->GetContext(),
											  context->GetDevice(),
											  Width, 
											  Height,
											  1,
											  imgFormat,
											  OpenCL::Image::Type::Texture2D,
											  access);
}

void UCLImageObject::Initialize2DArray(const TObjectPtr<UCLContextObject>& context, 
									   int32_t width, 
									   int32_t height, 
									   int32_t slices, 
									   UCLImageFormat format, 
									   UCLAccessType type)
{
	Width = width;
	Height = height;
	Depth = slices;
	Type = UCLImageType::Texture2DArray;

	OpenCL::AccessType access = AccessToCLAccess(type);
	OpenCL::Image::Format imgFormat = FormatToCLFormat(format);

	mpImage = std::make_shared<OpenCL::Image>(context->GetContext(),
											  context->GetDevice(),
											  Width, 
											  Height,
											  slices,
											  imgFormat,
											  OpenCL::Image::Type::Texture2DArray,
											  access);
}