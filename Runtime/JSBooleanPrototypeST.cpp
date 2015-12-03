#include "Runtime/JSBooleanPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSBooleanPrototype* JSBooleanPrototype::Create(JSGlobalObject* globalObject, JSValue prototype)
	{
		return new JSBooleanPrototype(globalObject, prototype);
	}
}