#ifndef MOENJIN_RUNTIME_JSFUNCTION_H
#define MOENJIN_RUNTIME_JSFUNCTION_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"

namespace MoEnjin
{
	class JSExecutionEnvironment;
	class JSStackFrame;
	class JSFunction : public JSObject
	{
		public:
			JS_CLASS();

			static JSValueImpl CallHostFunctionAsConstructor(JSStackFrame*);

			static mo_bool IsCallableImpl();
			static mo_bool ImplementsHasInstanceImpl();

			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSFunction* Create(JSStackFrame* exec, const UTF8String& name, mo_int32 length, NativeFunctionPtr function, NativeFunctionPtr constructor = CallHostFunctionAsConstructor, mo_bool isStrictMode = mo_false, mo_bool isHostFunction = mo_true);
			static JSFunction* Create(JSStackFrame* exec, JSValue prototype, const UTF8String& name, JSExecutionEnvironment* scope, mo_int32 length, NativeFunctionPtr function, NativeFunctionPtr constructor = CallHostFunctionAsConstructor, mo_bool isStrictMode = mo_false, mo_bool isHostFunction = mo_true);

		protected:
			static mo_bool GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot);
			static mo_bool GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);

			static mo_bool DefineOwnPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException);
			static mo_bool DeletePropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, mo_bool throwException, mo_bool force = mo_false);

			static void PutImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSValue value, mo_unsigned attributes);

		public:
			const UTF8String& GetName(JSStackFrame* exec) const;
			const UTF8String& GetDisplayName(JSStackFrame* exec) const;
			const UTF8String GetDebuggerName(JSStackFrame* exec) const;

			mo_uint32 GetParameterCount() const { return mParameterCount; }

			NativeFunctionPtr GetFunction() const { return mNativeFunction; }
			NativeFunctionPtr GetConstructor() const { return mNativeConstructor; }

			JSExecutionEnvironment* GetScope() const { return mScope; }

			void SetScope(JSExecutionEnvironment* scope) { mScope = scope; }
			void SetIsStrictMode(mo_bool value) { mIsStrictMode = value; }
			void SetIsHostFunction(mo_bool value) { mIsHostFunction = value; }

			mo_bool IsStrictMode() const { return mIsStrictMode; }
			mo_bool IsHostFunction() const { return mIsHostFunction; }

			mo_bool HasFunction() const { return (mNativeFunction != mo_null); }
			mo_bool HasConstructor() const { return (mNativeConstructor != mo_null); }
			mo_bool HasParameters() const { return (mParameterCount != 0); }

		private:
			JSFunction(JSGlobalObject* globalObject, JSValue prototype, const UTF8String& name, mo_uint32 length, JSExecutionEnvironment* scope, NativeFunctionPtr function, NativeFunctionPtr constructor, mo_bool isStrictMode, mo_bool isHostFunction);

		private:
			UTF8String mName;
			mo_uint32 mParameterCount;
			NativeFunctionPtr mNativeFunction;
			NativeFunctionPtr mNativeConstructor;
			JSExecutionEnvironment* mScope;
			mo_bool mIsStrictMode;
			mo_bool mIsHostFunction;
	};
}

#endif