#include "Runtime/JSNativeErrorConstructor.h"
#include "Runtime/JSNativeErrorPrototype.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSCallInfo JSNativeErrorConstructor::GetCallInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSNativeErrorConstructor::Call);
	}

	JSCallInfo JSNativeErrorConstructor::GetConstructInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSNativeErrorConstructor::Construct);
	}

	JSValueImpl JSNativeErrorConstructor::Construct(JSStackFrame* exec)
	{
		JSValue message = exec->HasArguments() ? exec->GetArgument(0) : jsUndef();
		JSNativeErrorConstructor* ctor = mo_cast<JSNativeErrorConstructor*>(exec->GetCallee());
		JSErrorInstance* errObj = JSErrorInstance::Create(exec, ctor->GetErrorPrototype(exec), message);

		return JSValue::Encode(errObj);
	}

	JSValueImpl JSNativeErrorConstructor::Call(JSStackFrame* exec)
	{
		JSValue message = exec->HasArguments() ? exec->GetArgument(0) : jsUndef();
		JSNativeErrorConstructor* ctor = mo_cast<JSNativeErrorConstructor*>(exec->GetCallee());
		JSErrorInstance* errObj = JSErrorInstance::Create(exec, ctor->GetErrorPrototype(exec), message);

		return JSValue::Encode(errObj);
	}

	JSNativeErrorConstructor* JSNativeErrorConstructor::Create(JSStackFrame* exec, const UTF8String& name)
	{
		JSNativeErrorConstructor* ctor = new JSNativeErrorConstructor(exec->GetGlobalObject(), name);
		JSNativeErrorPrototype* prototype = JSNativeErrorPrototype::Create(exec, exec->GetGlobalObject()->GetErrorPrototype(), name, ctor);

		ctor->Put(exec, KnownIdentifiers::prototype, prototype, JS_PROP_ATTR_FIXED);
		ctor->Put(exec, KnownIdentifiers::length, jsNumber(1), JS_PROP_ATTR_FIXED);

		return ctor;
	}
}