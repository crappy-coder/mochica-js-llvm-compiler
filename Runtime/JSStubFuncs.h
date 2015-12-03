#ifndef MOENJIN_RUNTIME_JSSTUBFUNCS_H
#define MOENJIN_RUNTIME_JSSTUBFUNCS_H

#include "Platform/Configuration.h"
#include "Runtime/BitOps.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSExecutionEnvironment;
	class JSFunction;
	class JSObject;
	class JSStackFrame;

	class JSStubFuncs
	{
		MO_STATIC_CLASS(JSStubFuncs);

		public:
			static mo_bool DeleteProperty(JSStackFrame* exec, JSValue baseValue, const UTF8String& propertyName, mo_bool inStrictMode = mo_false);

			static UTF8String ResolvePropertyName(JSStackFrame* exec, JSValue property);

			
			static void BindVariable(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool inStrictMode = mo_false);
			static void BindExceptionVariable(JSStackFrame* exec, const UTF8String& name, JSValue value);

			static void BindFunction(JSStackFrame* exec, JSFunction* function);
			static void BindFunctionParameter(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool inStrictMode = mo_false);
			static void BindFunctionArgumentsProperty(JSStackFrame* exec, JSFunction* function, const mo_utf8stringptr* parameterNames, mo_uint32 parameterNamesLen);


			static JSFunction* ConstructFunction(JSStackFrame* exec, JSExecutionEnvironment* scope, const UTF8String& name, mo_uint32 length, NativeFunctionPtr nativeFunction, mo_bool inStrictMode);

			static JSValue IncrementValue(JSStackFrame* exec, JSValue baseValue, const UTF8String& propertyName, mo_int32 amount, JSValue& oldValue);

			static JSValue Add(JSStackFrame* exec, JSValue x, JSValue y);
			static JSValue BitOpValue(JSStackFrame* exec, JSValue x, JSValue y, BitOp op);

			static mo_bool IsLess(JSStackFrame* exec, JSValue x, JSValue y, mo_bool leftFirstEval);
			static mo_bool IsLessOrEqual(JSStackFrame* exec, JSValue x, JSValue y, mo_bool leftFirstEval);

			static mo_bool AreEqual(JSStackFrame* exec, JSValue x, JSValue y);
			static mo_bool AreStrictEqual(JSStackFrame* exec, JSValue x, JSValue y);

	};
}

#endif