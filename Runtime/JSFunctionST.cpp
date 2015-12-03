#include "Runtime/JSFunction.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSScriptEnvironment.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	// ECMA 13.2 - Creating Function Objects
	JSFunction* JSFunction::Create(JSStackFrame* exec, const UTF8String& name, mo_int32 length, NativeFunctionPtr function, NativeFunctionPtr constructor, mo_bool isStrictMode, mo_bool isHostFunction)
	{
		JSGlobalObject* globalObject = exec->GetGlobalObject();
		return JSFunction::Create(exec, globalObject->GetFunctionPrototype(), name, globalObject->GetEnvironment(), length, function, constructor, isStrictMode, isHostFunction);
	}

	// ECMA 13.2 - Creating Function Objects
	JSFunction* JSFunction::Create(JSStackFrame* exec, JSValue prototype, const UTF8String& name, JSExecutionEnvironment* scope, mo_int32 length, NativeFunctionPtr function, NativeFunctionPtr constructor, mo_bool isStrictMode, mo_bool isHostFunction)
	{
		JSFunction* jsFunc = new JSFunction(exec->GetGlobalObject(), prototype, name, length, scope, function, constructor, isStrictMode, isHostFunction);

		JSObject* defaultPrototype = JSObject::Create(exec);
		defaultPrototype->Put(exec, KnownIdentifiers::constructor, jsFunc, JSPropertyAttributeDontEnum);

		jsFunc->Put(exec, KnownIdentifiers::prototype, defaultPrototype, JSPropertyAttributeDontEnum | JSPropertyAttributeDontDelete);
		jsFunc->Put(exec, KnownIdentifiers::length, jsNumber(length), JSPropertyAttributeDontEnum | JSPropertyAttributeDontDelete | JSPropertyAttributeReadOnly);

		return jsFunc;
	}

	mo_bool JSFunction::GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		// TODO - handle strict mode property access (see ECMA 15.3.5.4)
		return JSObject::GetOwnPropertySlotImpl(exec, obj, propertyName, slot);
	}

	mo_bool JSFunction::GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		// TODO - handle strict mode property access (see ECMA 15.3.5.4)
		return JSObject::GetOwnPropertyDescriptorImpl(exec, obj, propertyName, descriptor);
	}

	mo_bool JSFunction::DefineOwnPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException)
	{
		return JSObject::DefineOwnPropertyImpl(exec, obj, propertyName, descriptor, throwException);
	}

	mo_bool JSFunction::DeletePropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, mo_bool throwException, mo_bool force)
	{
		JSFunction* me = JSConvert::Cast<JSFunction*>(obj);

		// don't allow these properties to be deleted for js functions, unless
		// it's being forcibly deleted internally
		if (!force && 
			(propertyName == KnownIdentifiers::arguments	||
			 propertyName == KnownIdentifiers::caller		||
			 propertyName == KnownIdentifiers::prototype	||
			 propertyName == KnownIdentifiers::length)) 
		{ 
			return mo_false; 
		}

		return JSObject::DeletePropertyImpl(exec, obj, propertyName, throwException, force);
	}

	void JSFunction::PutImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSValue value, mo_unsigned attributes)
	{
		// TODO - handle strict mode property access (see ECMA 15.3.5.4)
		JSObject::PutImpl(exec, obj, propertyName, value, attributes);
	}

	mo_bool JSFunction::IsCallableImpl()
	{
		return mo_true;
	}

	mo_bool JSFunction::ImplementsHasInstanceImpl()
	{
		return mo_true;
	}

	JSCallInfo JSFunction::GetCallInfoImpl(JSStackFrame* exec, JSObject* obj)
	{
		JSFunction* me = JSConvert::Cast<JSFunction*>(obj);

		if(me->IsHostFunction())
			return jsHostCallInfo(me->GetFunction());

		return jsScriptCallInfo(me->GetFunction(), me->GetScope());
	}

	JSCallInfo JSFunction::GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj)
	{
		JSFunction* me = JSConvert::Cast<JSFunction*>(obj);

		if(me->IsHostFunction())
			return jsHostCallInfo(me->GetConstructor());

		return jsScriptCallInfo(me->GetConstructor(), me->GetScope());
	}

	JSValueImpl JSFunction::CallHostFunctionAsConstructor(JSStackFrame*)
	{
		jsThrowNativeErrorWithMessage(kJSTypeError, "Cannot call function as a constructor.");
		return jsUndef();
	}
}