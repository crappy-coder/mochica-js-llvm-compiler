#ifndef MOENJIN_RUNTIME_JSVALUEDATA_H
#define MOENJIN_RUNTIME_JSVALUEDATA_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	typedef mo_int64 JSValueImpl;

	class JSObject;

	union JSValueData
	{
		mo_int64 asInt64;
#if defined(MO_PLATFORM_USE_JSVALUE_32)
		mo_double asDouble;
#elif defined(MO_PLATFORM_USE_JSVALUE_64)
		JSObject* asPtr;
#endif
		struct {
			mo_int32 payload;
			mo_int32 tag;
		} asBits;
	};
}

#endif