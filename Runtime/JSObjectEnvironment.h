#ifndef MOENJIN_RUNTIME_JSOBJECTENVIRONMENT_H
#define MOENJIN_RUNTIME_JSOBJECTENVIRONMENT_H

#include "Platform/Configuration.h"
#include "Runtime/JSExecutionEnvironment.h"

namespace MoEnjin
{
	class JSObject;
	class JSObjectEnvironment : public JSExecutionEnvironment
	{
		MO_NON_COPYABLE_CLASS(JSObjectEnvironment);

		public:
			JSObject* GetBoundObject() const { return mBoundObject; }
			mo_bool GetShouldProvideThis() const { return mShouldProvideThis; }

			void CreateImmutableBinding(JSStackFrame* exec, const UTF8String& name) { /** no op **/ }
			void InitializeImmutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value) { /** no op **/ }

			void CreateMutableBinding(JSStackFrame* exec, const UTF8String& name, mo_bool canDelete = mo_true);
			void SetMutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool strictMode = mo_false);

			mo_bool HasBinding(JSStackFrame* exec, const UTF8String& name);
			mo_bool DeleteBinding(JSStackFrame* exec, const UTF8String& name);
			JSValue GetBindingValue(JSStackFrame* exec, const UTF8String& name, mo_bool strictMode = mo_false);
			JSValue GetImplicitThisValue(JSStackFrame* exec);

			static JSObjectEnvironment* Create(JSGlobalObject* globalObject, JSObject* boundObject, mo_bool provideThis = mo_false, JSExecutionEnvironment* parent = mo_null);

		private:
			JSObjectEnvironment(JSGlobalObject* globalObject, JSObject* boundObject, mo_bool provideThis = mo_false, JSExecutionEnvironment* parent = mo_null);

		private:
			JSObject* mBoundObject;
			mo_bool mShouldProvideThis;
	};
}

#endif