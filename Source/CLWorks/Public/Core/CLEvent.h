#pragma once

namespace OpenCL
{
	class CLWORKS_API Event
	{
	public:
		Event();

		Event(cl_event event);
	public:
		operator cl_event() const { return mpEvent; }

		cl_event Get() const { return mpEvent; }
	public:
		cl_event mpEvent;
	};
}