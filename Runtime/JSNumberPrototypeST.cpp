#include "Runtime/JSNumberPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSNumberPrototype* JSNumberPrototype::Create(JSGlobalObject* globalObject, JSValue prototype)
	{
		return new JSNumberPrototype(globalObject, prototype);
	}
}