#pragma once

namespace OpenCL
{
	enum class AccessType : uint8_t
	{
		INVALID = 0,

		READ_ONLY,
		WRITE_ONLY,
		READ_WRITE,

		COUNT
	};

	enum class MemoryStrategy : uint8_t 
	{
		INVALID,
		
		COPY_ONCE,
		STREAM,
		ZERO_COPY,

		COUNT
	};
}