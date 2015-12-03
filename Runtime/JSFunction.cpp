#include "Runtime/JSFunction.h"
#include "Runtime/JSExecutionEnvironment.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSFunction, Function, JSObject);

	JSFunction::JSFunction(JSGlobalObject* globalObject, JSValue prototype, const UTF8String& name, mo_uint32 length, JSExecutionEnvironment* scope, NativeFunctionPtr function, NativeFunctionPtr constructor, mo_bool isStrictMode, mo_bool isHostFunction)
		: JSObject(globalObject, JSObjectTypeFunction, prototype, &ClassInfo)
		, mScope(scope)
		, mName(name)
		, mParameterCount(length)
		, mNativeFunction(function)
		, mNativeConstructor(constructor)
		, mIsStrictMode(isStrictMode)
		, mIsHostFunction(isHostFunction) { }

	const UTF8String& JSFunction::GetName(JSStackFrame* exec) const
	{
		// TODO - return name from property storage once thats implemented
		return mName;
	}

	const UTF8String& JSFunction::GetDisplayName(JSStackFrame* exec) const
	{
		return GetName(exec);
	}

	const UTF8String JSFunction::GetDebuggerName(JSStackFrame* exec) const
	{
		const UTF8String displayName = GetDisplayName(exec);

		if(!displayName.IsEmpty())
			return displayName;

		const UTF8String actualName = GetName(exec);

		if(!actualName.IsEmpty())
			return actualName;

		// TODO - return inferred name?
		return "";
	}
}