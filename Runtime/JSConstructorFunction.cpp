#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSConstructorFunction, Function, JSObject);

	JSConstructorFunction::JSConstructorFunction(JSGlobalObject* globalObject, JSValue prototype, const UTF8String& name)
		: JSObject(globalObject, JSObjectTypeObject, prototype, &ClassInfo)
		, mName(name) { }

	JSConstructorFunction::JSConstructorFunction(JSGlobalObject* globalObject, JSValue prototype, const JSClassInfo* classInfo, const UTF8String& name)
		: JSObject(globalObject, JSObjectTypeObject, prototype, classInfo)
		, mName(name) { }

	const UTF8String& JSConstructorFunction::GetName(JSStackFrame* exec) const
	{
		return mName;
	}

	const UTF8String& JSConstructorFunction::GetDisplayName(JSStackFrame* exec) const
	{
		return mName;
	}

	const UTF8String JSConstructorFunction::GetDebuggerName(JSStackFrame* exec) const
	{
		return "";
	}
}