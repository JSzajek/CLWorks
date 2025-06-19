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
}