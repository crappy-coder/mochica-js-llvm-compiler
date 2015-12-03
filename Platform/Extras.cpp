#include "Platform/Configuration.h"
#include "Platform/Extras.h"

extern "C" {
	void MoReportAssertionFailure(const char* file, int line, const char* function, const char* assertion)
	{
		if(assertion)
			MoEnjin::mo_debug_print("ASSERT FAILED: %s\n", assertion);
		else
			MoEnjin::mo_debug_print("UNREACHABLE CODE SHOULD NOT HAVE BEEN REACHED\n");

#if defined(MO_PLATFORM_WINDOWS)
		_CrtDbgReport(_CRT_WARN, file, line, NULL, "%s\n", function);
#else
		MoEnjin::mo_debug_print("%s(%d) : %s\n", file, line, function);
#endif
	}

	void MoReportError(const char* file, int line, const char* function, const char* msg)
	{
		if(msg)
			MoEnjin::mo_debug_print("ERROR: %s\n", msg);

#if defined(MO_PLATFORM_WINDOWS)
		_CrtDbgReport(_CRT_ERROR, file, line, NULL, "%s\n", msg);
#else
		MoEnjin::mo_debug_print("%s(%d) : %s\n", file, line, function);
#endif
	}
}