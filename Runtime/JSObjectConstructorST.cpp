#include "Runtime/JSObjectConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSObjectPrototype.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSCallInfo JSObjectConstructor::GetCallInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSObjectConstructor::Call);
	}

	JSCallInfo JSObjectConstructor::GetConstructInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSObjectConstructor::Construct);
	}

	JSValueImpl JSObjectConstructor::Construct(JSStackFrame* exec)
	{
		JSValue arg = (exec->HasArguments() ? exec->GetArgument(0) : jsUndef());

		if(!arg.IsNullOrUndefined())
		{
			if(arg.IsNativeObject())
				return JSValue::Encode(arg);

			if(arg.IsString() || arg.IsBoolean() || arg.IsNumber())
				return JSValue::Encode(arg.ToObject(exec));
		}

		MO_ASSERT(arg.IsNullOrUndefined());

		return JSValue::Encode(JSObject::Create(exec));
	}

	JSValueImpl JSObjectConstructor::Call(JSStackFrame* exec)
	{
		if(exec->GetArgumentCount() == 0 || exec->GetArgument(0).IsNullOrUndefined())
			return Construct(exec);

		return JSValue::Encode(exec->GetArgument(0).ToObject(exec));
	}

	JSObjectConstructor* JSObjectConstructor::Create(JSStackFrame* exec)
	{
		JSObjectConstructor* jsObjectConstructor = new JSObjectConstructor(exec->GetGlobalObject());

		jsObjectConstructor->Put(exec, KnownIdentifiers::prototype, exec->GetGlobalObject()->GetObjectPrototype(), JS_PROP_ATTR_FIXED);
		jsObjectConstructor->Put(exec, KnownIdentifiers::length, jsNumber(1), JS_PROP_ATTR_FIXED);

		return jsObjectConstructor;
	}
}