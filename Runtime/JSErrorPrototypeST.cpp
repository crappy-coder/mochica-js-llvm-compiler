#include "Runtime/JSErrorPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSErrorPrototype* JSErrorPrototype::Create(JSGlobalObject* globalObject, JSValue prototype)
	{
		return new JSErrorPrototype(globalObject, prototype);
	}
}