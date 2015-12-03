#include "Runtime/JSFunctionConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSObjectPrototype.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSCallInfo JSFunctionConstructor::GetCallInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSFunctionConstructor::Call);
	}

	JSCallInfo JSFunctionConstructor::GetConstructInfoImpl(JSStackFrame*, JSObject*)
	{
		return jsHostCallInfo(JSFunctionConstructor::Construct);
	}

	JSValueImpl JSFunctionConstructor::Construct(JSStackFrame* exec)
	{
		jsThrowNativeErrorWithMessage(kJSNotImplementedError, "Cannot construct a function from a new Function(..) expression.");
		return jsUndef();
	}

	JSValueImpl JSFunctionConstructor::Call(JSStackFrame* exec)
	{
		jsThrowNativeErrorWithMessage(kJSNotImplementedError, "Cannot call a function constructor as a function.");
		return jsUndef();
	}

	JSFunctionConstructor* JSFunctionConstructor::Create(JSStackFrame* exec)
	{
		JSFunctionConstructor* jsFunctionConstructor = new JSFunctionConstructor(exec->GetGlobalObject());

		jsFunctionConstructor->Put(exec, KnownIdentifiers::prototype, exec->GetGlobalObject()->GetFunctionPrototype(), JS_PROP_ATTR_FIXED);
		jsFunctionConstructor->Put(exec, KnownIdentifiers::length, jsNumber(1), JS_PROP_ATTR_FIXED);

		return jsFunctionConstructor;
	}
}