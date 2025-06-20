#pragma once

namespace OpenCL
{
	enum class AccessType : uint8_t
	{
		READ_ONLY,
		WRITE_ONLY,
		READ_WRITE,

		COUNT
	};

	enum class MemoryStrategy : uint8_t 
	{
		COPY_ONCE,
		STREAM,
		ZERO_COPY,

		COUNT
	};
}