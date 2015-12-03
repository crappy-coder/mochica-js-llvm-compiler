#include "Runtime/JSNativeErrorConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSNativeErrorConstructor, Function, JSConstructorFunction);

	JSNativeErrorConstructor::JSNativeErrorConstructor(JSGlobalObject* globalObject, const UTF8String& name)
		: JSConstructorFunction(globalObject, globalObject->GetFunctionPrototype(), &ClassInfo, name) { }

	JSValue JSNativeErrorConstructor::GetErrorPrototype(JSStackFrame* exec) const
	{
		return Get(exec, KnownIdentifiers::prototype);
	}
}