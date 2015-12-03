#ifndef MOENJIN_RUNTIME_JSNATIVEEXCEPTIONHANDLER_H
#define MOENJIN_RUNTIME_JSNATIVEEXCEPTIONHANDLER_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	struct JSNativeExceptionHandler
	{
		jmp_buf jmpInfo;
		JSValueImpl exceptionValue;
		JSNativeExceptionHandler* previous;
	};
}

#endif