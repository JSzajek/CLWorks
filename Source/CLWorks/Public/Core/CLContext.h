#pragma once

#include "Core/CLDevice.h"

namespace OpenCL
{
	enum class Interop
	{
		None,
		Vulkan,
		DirectX,
	};

	struct CLWORKS_API ContextProperties
	{
	public:
		Interop mInteropType = Interop::None;
	};

	class CLWORKS_API Context
	{
	public:
		Context();

		Context(const OpenCL::Device& device,
				const ContextProperties& properties = {});

		Context(cl_device_id device, 
				const ContextProperties& properties = {});

		~Context();
	public:
		cl_context Get() const { return mpContext; };
		
		void PrintSupportedImageFormats(cl_mem_flags mem_flags);
	private:
		void Initialize(cl_device_id device, 
						const ContextProperties& properties);
	private:
		cl_context mpContext = nullptr;
	};
}