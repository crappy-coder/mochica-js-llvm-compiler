#include "Runtime/JSNativeErrorPrototype.h"

namespace MoEnjin
{
	JSNativeErrorPrototype::JSNativeErrorPrototype(JSGlobalObject* globalObject, JSValue prototype)
		: JSErrorPrototype(globalObject, prototype) { }
}