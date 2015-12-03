#include "Runtime/JSStringConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSObjectPrototype.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSCallInfo JSStringConstructor::GetCallInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSStringConstructor::Call);
	}

	JSCallInfo JSStringConstructor::GetConstructInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSStringConstructor::Construct);
	}

	JSValueImpl JSStringConstructor::Construct(JSStackFrame* exec)
	{
		if(exec->HasArguments())
			return JSValue::Encode(JSStringInstance::Create(exec, exec->GetArgument(0).ToString(exec)));

		return JSValue::Encode(JSStringInstance::Create(exec));
	}

	JSValueImpl JSStringConstructor::Call(JSStackFrame* exec)
	{
		jsThrowNativeErrorWithMessage(kJSNotImplementedError, "Cannot call a string constructor as a function.");
		return jsNumber(0);
	}

	JSStringConstructor* JSStringConstructor::Create(JSStackFrame* exec)
	{
		JSStringConstructor* jsStringConstructor = new JSStringConstructor(exec->GetGlobalObject());

		jsStringConstructor->Put(exec, KnownIdentifiers::prototype, exec->GetGlobalObject()->GetStringPrototype(), JS_PROP_ATTR_FIXED);
		jsStringConstructor->Put(exec, KnownIdentifiers::length, jsNumber(1), JS_PROP_ATTR_FIXED);

		return jsStringConstructor;
	}
}