#pragma once

#include "OpenCLLib.h"

namespace OpenCL
{
	class CLWORKS_API Event
	{
		using Callback = std::function<void()>;

	public:
		Event();

		Event(cl_event event);
	public:
		operator cl_event() const { return mpEvent; }

		cl_event Get() const { return mpEvent; }

		void SetOnCompleteCallback(Callback&& callback);
	public:
		cl_event mpEvent;

		void* mpCaller = nullptr;
		Callback mCallback;
	private:
		static void CL_CALLBACK OnEventCompleteStatic(cl_event Event, cl_int ExecStatus, void* UserData);
	};
}