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

		Context(const DevicePtr& device,
				const ContextProperties& properties = {});

		~Context();
	public:
		operator cl_context() const { return mpContext; }
		cl_context Get() const { return mpContext; };
		
		void PrintSupportedImageFormats(cl_mem_flags mem_flags);
	private:
		void Initialize(cl_device_id device, 
						const ContextProperties& properties);
	private:
		cl_context mpContext = nullptr;
	};

	using ContextPtr = std::shared_ptr<OpenCL::Context>;

	inline static ContextPtr MakeContext(const DevicePtr& device,
										 const ContextProperties& properties = {})
	{
		return std::make_shared<OpenCL::Context>(device, properties);
	}
}