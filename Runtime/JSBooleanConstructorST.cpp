#include "Runtime/JSBooleanConstructor.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSCallInfo JSBooleanConstructor::GetCallInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSBooleanConstructor::Call);
	}

	JSCallInfo JSBooleanConstructor::GetConstructInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSBooleanConstructor::Construct);
	}

	JSValueImpl JSBooleanConstructor::Construct(JSStackFrame* exec)
	{
		JSValue arg = jsBool((exec->HasArguments() ? exec->GetArgument(0) : jsUndef()).ToBoolean(exec));
		JSBooleanInstance* boolObj = JSBooleanInstance::Create(exec, arg);

		return JSValue::Encode(boolObj);
	}

	JSValueImpl JSBooleanConstructor::Call(JSStackFrame* exec)
	{
		JSValue arg = jsBool((exec->HasArguments() ? exec->GetArgument(0) : jsUndef()).ToBoolean(exec));

		return JSValue::Encode(arg);
	}

	JSBooleanConstructor* JSBooleanConstructor::Create(JSStackFrame* exec)
	{
		JSBooleanConstructor* jsBooleanConstructor = new JSBooleanConstructor(exec->GetGlobalObject());

		jsBooleanConstructor->Put(exec, KnownIdentifiers::prototype, exec->GetGlobalObject()->GetBooleanPrototype(), JS_PROP_ATTR_FIXED);
		jsBooleanConstructor->Put(exec, KnownIdentifiers::length, jsNumber(1), JS_PROP_ATTR_FIXED);

		return jsBooleanConstructor;
	}
}