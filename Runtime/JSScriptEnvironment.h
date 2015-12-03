#ifndef MOENJIN_RUNTIME_JSSCRIPTENVIRONMENT_H
#define MOENJIN_RUNTIME_JSSCRIPTENVIRONMENT_H

#include "Platform/Configuration.h"
#include "Runtime/JSExecutionEnvironment.h"

namespace MoEnjin
{
	class JSScriptEnvironment : public JSExecutionEnvironment
	{
		MO_NON_COPYABLE_CLASS(JSScriptEnvironment);

		public:
			void CreateImmutableBinding(JSStackFrame* exec, const UTF8String& name);
			void InitializeImmutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value);

			void CreateMutableBinding(JSStackFrame* exec, const UTF8String& name, mo_bool canDelete = mo_true);
			void SetMutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool strictMode = mo_false);

			mo_bool HasBinding(JSStackFrame* exec, const UTF8String& name);
			mo_bool DeleteBinding(JSStackFrame* exec, const UTF8String& name);
			JSValue GetBindingValue(JSStackFrame* exec, const UTF8String& name, mo_bool strictMode = mo_false);
			JSValue GetImplicitThisValue(JSStackFrame* exec);

			static JSScriptEnvironment* Create(JSGlobalObject* globalObject, JSExecutionEnvironment* parent = mo_null);

		private:
			JSScriptEnvironment(JSGlobalObject* globalObject, JSExecutionEnvironment* parent = mo_null);
	};
}

#endif