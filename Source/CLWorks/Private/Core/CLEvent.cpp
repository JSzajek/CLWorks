#include "Core/CLEvent.h"

namespace OpenCL
{
	Event::Event()
		: mpEvent()
	{

	}

	Event::Event(cl_event event)
		: mpEvent(event)
	{
	}

	void Event::SetOnCompleteCallback(Callback&& callback)
	{
		mCallback = std::move(callback);

		clSetEventCallback(mpEvent, CL_COMPLETE, &Event::OnEventCompleteStatic, this);
	}

	void CL_CALLBACK Event::OnEventCompleteStatic(cl_event _event, cl_int status, void* data)
	{
		auto* Self = static_cast<Event*>(data);
		if (Self && Self->mCallback)
		{
			Self->mCallback();

			// Clean Up With Completion
			Self->mCallback = {};
		}
	}
}