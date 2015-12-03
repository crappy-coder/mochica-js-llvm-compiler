#include "Runtime/JSErrorConstructor.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSCallInfo JSErrorConstructor::GetCallInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSErrorConstructor::Call);
	}

	JSCallInfo JSErrorConstructor::GetConstructInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSErrorConstructor::Construct);
	}

	JSValueImpl JSErrorConstructor::Construct(JSStackFrame* exec)
	{
		JSValue message = exec->HasArguments() ? exec->GetArgument(0) : jsUndef();
		JSErrorInstance* errObj = JSErrorInstance::Create(exec, message);

		return JSValue::Encode(errObj);
	}

	JSValueImpl JSErrorConstructor::Call(JSStackFrame* exec)
	{
		JSValue message = exec->HasArguments() ? exec->GetArgument(0) : jsUndef();
		JSErrorInstance* errObj = JSErrorInstance::Create(exec, message);

		return JSValue::Encode(errObj);
	}

	JSErrorConstructor* JSErrorConstructor::Create(JSStackFrame* exec)
	{
		JSErrorConstructor* ctor = new JSErrorConstructor(exec->GetGlobalObject());

		ctor->Put(exec, KnownIdentifiers::prototype, exec->GetGlobalObject()->GetErrorPrototype(), JS_PROP_ATTR_FIXED);
		ctor->Put(exec, KnownIdentifiers::length, jsNumber(1), JS_PROP_ATTR_FIXED);

		return ctor;
	}
}