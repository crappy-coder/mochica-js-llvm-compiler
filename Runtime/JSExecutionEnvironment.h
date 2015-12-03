#ifndef MOENJIN_RUNTIME_JSEXECUTIONENVIRONMENT_H
#define MOENJIN_RUNTIME_JSEXECUTIONENVIRONMENT_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSStackFrame;
	class JSExecutionEnvironment : public JSObject
	{
		MO_NON_COPYABLE_CLASS(JSExecutionEnvironment);

		public:
			JS_CLASS();

		public:
			JSExecutionEnvironment* GetParent() const { return mParent; }

			JSValue GetIdentifierReference(JSStackFrame* exec, const UTF8String& name, mo_bool strictMode = mo_false);

			virtual void CreateImmutableBinding(JSStackFrame* exec, const UTF8String& name) = 0;
			virtual void InitializeImmutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value) = 0;

			virtual void CreateMutableBinding(JSStackFrame* exec, const UTF8String& name, mo_bool canDelete = mo_true) = 0;
			virtual void SetMutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool strictMode = mo_false) = 0;

			virtual mo_bool HasBinding(JSStackFrame* exec, const UTF8String& name) = 0;
			virtual mo_bool DeleteBinding(JSStackFrame* exec, const UTF8String& name) = 0;
			virtual JSValue GetBindingValue(JSStackFrame* exec, const UTF8String& name, mo_bool strictMode = mo_false) = 0;
			virtual JSValue GetImplicitThisValue(JSStackFrame* exec) = 0;

		protected:
			JSExecutionEnvironment(JSGlobalObject* globalObject, JSExecutionEnvironment* parent = mo_null);

		private:
			JSExecutionEnvironment* mParent;
	};
}

#endif