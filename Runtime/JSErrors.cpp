#include "Runtime/JSErrors.h"
#include "Runtime/JSErrorInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSNativeErrorConstructor.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	JSObject* jsCreateError(JSStackFrame* exec, const UTF8String& message)
	{
		return JSErrorInstance::Create(exec, message);
	}

	JSObject* jsCreateEvalError(JSStackFrame* exec, const UTF8String& message)
	{
		return JSErrorInstance::Create(exec, exec->GetGlobalObject()->GetEvalErrorConstructor()->GetErrorPrototype(exec), message);
	}

	JSObject* jsCreateRangeError(JSStackFrame* exec, const UTF8String& message)
	{
		return JSErrorInstance::Create(exec, exec->GetGlobalObject()->GetRangeErrorConstructor()->GetErrorPrototype(exec), message);
	}

	JSObject* jsCreateReferenceError(JSStackFrame* exec, const UTF8String& message)
	{
		return JSErrorInstance::Create(exec, exec->GetGlobalObject()->GetReferenceErrorConstructor()->GetErrorPrototype(exec), message);
	}

	JSObject* jsCreateSyntaxError(JSStackFrame* exec, const UTF8String& message)
	{
		return JSErrorInstance::Create(exec, exec->GetGlobalObject()->GetSyntaxErrorConstructor()->GetErrorPrototype(exec), message);
	}

	JSObject* jsCreateTypeError(JSStackFrame* exec, const UTF8String& message)
	{
		return JSErrorInstance::Create(exec, exec->GetGlobalObject()->GetTypeErrorConstructor()->GetErrorPrototype(exec), message);
	}

	JSObject* jsCreateURIError(JSStackFrame* exec, const UTF8String& message)
	{
		return JSErrorInstance::Create(exec, exec->GetGlobalObject()->GetURIErrorConstructor()->GetErrorPrototype(exec), message);
	}

	JSObject* jsCreateMissingArgumentsError(JSStackFrame* exec)
	{
		return jsCreateTypeError(exec, "Not enough arguments.");
	}

	JSObject* jsCreateInvalidParamError(JSStackFrame* exec, const mo_utf8stringptr op, JSValue value)
	{
		UTF8String message = "'" + value.ToString(exec)->GetValue() + "' is not a valid argument for '" + op + "'";
		JSObject* ex = jsCreateTypeError(exec, message);

		mo_cast<JSErrorInstance*>(ex)->SetShowSourceInMessage(mo_true);
		return ex;
	}

	JSObject* jsCreateStackOverflowError(JSStackFrame* exec)
	{
		return jsCreateRangeError(exec, "Maximum call stack size exceeded");
	}

	JSObject* jsCreateOutOfMemoryError(JSStackFrame* exec)
	{
		return jsCreateError(exec, "Out of memory");
	}

	JSObject* jsCreateUndefinedVariableError(JSStackFrame* exec, const UTF8String& varName)
	{
		UTF8String message = "Cannot find the variable: " + varName;

		return jsCreateReferenceError(exec, message);
	}

	JSObject* jsCreateNotObjectError(JSStackFrame* exec, JSValue value)
	{
		UTF8String message = "'" + value.ToString(exec)->GetValue() + "' is not an object.";
		JSObject* ex = jsCreateTypeError(exec, message);

		mo_cast<JSErrorInstance*>(ex)->SetShowSourceInMessage(mo_true);
		return ex;
	}

	JSObject* jsCreateNotConstructorError(JSStackFrame* exec, JSValue value)
	{
		UTF8String message = "'" + value.ToString(exec)->GetValue() + "' is not a constructor.";
		JSObject* ex = jsCreateTypeError(exec, message);

		mo_cast<JSErrorInstance*>(ex)->SetShowSourceInMessage(mo_true);
		return ex;
	}

	JSObject* jsCreateNotFunctionError(JSStackFrame* exec, JSValue value)
	{
		UTF8String message = "'" + value.ToString(exec)->GetValue() + "' is not a function.";
		JSObject* ex = jsCreateTypeError(exec, message);

		mo_cast<JSErrorInstance*>(ex)->SetShowSourceInMessage(mo_true);
		return ex;
	}

	JSValue jsThrowError(JSStackFrame* exec, JSValue error)
	{
		if(error.IsObject())
			return jsThrowError(exec, error.ToObject(exec));

		exec->GetGlobalObject()->SetException(error);
		return error;
	}

	JSObject* jsThrowError(JSStackFrame* exec, JSObject* error)
	{
		exec->GetGlobalObject()->SetException(error);
		
		return error;
	}

	JSObject* jsThrowTypeError(JSStackFrame* exec)
	{
		return jsThrowError(exec, jsCreateTypeError(exec, "Type error"));
	}

	JSObject* jsThrowSyntaxError(JSStackFrame* exec)
	{
		return jsThrowError(exec, jsCreateSyntaxError(exec, "Syntax error"));
	}

	JSObject* jsThrowStackOverflowError(JSStackFrame* exec)
	{
		return jsThrowError(exec, jsCreateStackOverflowError(exec));
	}
}