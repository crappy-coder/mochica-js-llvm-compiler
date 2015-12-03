#ifndef MOENJIN_RUNTIME_JSERRORS_H
#define MOENJIN_RUNTIME_JSERRORS_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class JSStackFrame;
	class JSObject;
	class JSValue;

	JSObject* jsCreateError(JSStackFrame* exec, const UTF8String& message);
	JSObject* jsCreateEvalError(JSStackFrame* exec, const UTF8String& message);
	JSObject* jsCreateRangeError(JSStackFrame* exec, const UTF8String& message);
	JSObject* jsCreateReferenceError(JSStackFrame* exec, const UTF8String& message);
	JSObject* jsCreateSyntaxError(JSStackFrame* exec, const UTF8String& message);
	JSObject* jsCreateTypeError(JSStackFrame* exec, const UTF8String& message);
	JSObject* jsCreateURIError(JSStackFrame* exec, const UTF8String& message);
	JSObject* jsCreateMissingArgumentsError(JSStackFrame* exec);
	JSObject* jsCreateInvalidParamError(JSStackFrame* exec, const mo_utf8stringptr op, JSValue value);
	JSObject* jsCreateStackOverflowError(JSStackFrame* exec);
	JSObject* jsCreateOutOfMemoryError(JSStackFrame* exec);
	JSObject* jsCreateUndefinedVariableError(JSStackFrame* exec, const UTF8String& varName);
	JSObject* jsCreateNotObjectError(JSStackFrame* exec, JSValue value);
	JSObject* jsCreateNotConstructorError(JSStackFrame* exec, JSValue value);
	JSObject* jsCreateNotFunctionError(JSStackFrame* exec, JSValue value);

	JSValue jsThrowError(JSStackFrame* exec, JSValue error);
	JSObject* jsThrowError(JSStackFrame* exec, JSObject* error);
	JSObject* jsThrowTypeError(JSStackFrame* exec);
	JSObject* jsThrowSyntaxError(JSStackFrame* exec);
	JSObject* jsThrowStackOverflowError(JSStackFrame* exec);
}

#endif