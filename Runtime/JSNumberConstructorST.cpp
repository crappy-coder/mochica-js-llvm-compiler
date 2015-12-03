#include "Runtime/JSNumberConstructor.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSCallInfo JSNumberConstructor::GetCallInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSNumberConstructor::Call);
	}

	JSCallInfo JSNumberConstructor::GetConstructInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSNumberConstructor::Construct);
	}

	JSValueImpl JSNumberConstructor::Construct(JSStackFrame* exec)
	{
		JSValue arg = jsNumber((exec->HasArguments() ? exec->GetArgument(0) : jsNumber(0)).ToNumber(exec));
		JSNumberInstance* numObj = JSNumberInstance::Create(exec, arg);

		return JSValue::Encode(numObj);
	}

	JSValueImpl JSNumberConstructor::Call(JSStackFrame* exec)
	{
		jsThrowNativeErrorWithMessage(kJSNotImplementedError, "Cannot call a number constructor as a function.");
		return jsNumber(0);
	}

	JSNumberConstructor* JSNumberConstructor::Create(JSStackFrame* exec)
	{
		JSNumberConstructor* jsNumberConstructor = new JSNumberConstructor(exec->GetGlobalObject());

		jsNumberConstructor->Put(exec, KnownIdentifiers::prototype, exec->GetGlobalObject()->GetNumberPrototype(), JS_PROP_ATTR_FIXED);
		jsNumberConstructor->Put(exec, KnownIdentifiers::length, jsNumber(1), JS_PROP_ATTR_FIXED);

		return jsNumberConstructor;
	}
}