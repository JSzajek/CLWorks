#include "Objects/CLImageObject.h"

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

	OpenCL::AccessType access = OpenCL::AccessType::READ_WRITE;
	switch (type)
	{
		case UCLAccessType::READ_ONLY:
			access = OpenCL::AccessType::READ_ONLY;
			break;
		case UCLAccessType::WRITE_ONLY:
			access = OpenCL::AccessType::WRITE_ONLY;
			break;
		case UCLAccessType::READ_WRITE:
			access = OpenCL::AccessType::READ_WRITE;
			break;
	}

	OpenCL::Image::Format imgFormat = OpenCL::Image::Format::RGBA8;
	switch (format)
	{
		case UCLImageFormat::R8:
			imgFormat = OpenCL::Image::R8;
			break;
		case UCLImageFormat::RGBA8:
			imgFormat = OpenCL::Image::RGBA8;
			break;
		case UCLImageFormat::R16:
			imgFormat = OpenCL::Image::R16F;
			break;
		case UCLImageFormat::RGBA16:
			imgFormat = OpenCL::Image::RGBA16F;
			break;
		case UCLImageFormat::R32:
			imgFormat = OpenCL::Image::R32F;
			break;
		case UCLImageFormat::RGBA32:
			imgFormat = OpenCL::Image::RGBA32F;
			break;
	}

	mpImage = std::make_unique<OpenCL::Image>(context->GetContext(),
											  context->GetDevice(),
											  Width, 
											  Height,
											  1,
											  imgFormat,
											  OpenCL::Image::Type::Texture2D,
											  access);
}