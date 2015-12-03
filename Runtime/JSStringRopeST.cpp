#include "Runtime/JSStringRope.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JSStringRope* JSStringRope::Create(JSGlobalObject* globalObject)
	{
		return new JSStringRope(globalObject);
	}

	JSStringRope* JSStringRope::FromStrings(JSGlobalObject* globalObject, JSString* a, JSString* b)
	{
		return new JSStringRope(globalObject, a, b);
	}

	JSStringRope* JSStringRope::FromStrings(JSGlobalObject* globalObject, JSString* a, JSString* b, JSString* c)
	{
		return new JSStringRope(globalObject, a, b, c);
	}
}