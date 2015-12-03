#include "Runtime/JSObjectEnvironment.h"
#include "Runtime/JSObject.h"

namespace MoEnjin
{
	JSObjectEnvironment::JSObjectEnvironment(JSGlobalObject* globalObject, JSObject* boundObject, mo_bool provideThis, JSExecutionEnvironment* parent)
		: JSExecutionEnvironment(globalObject, parent)
		, mBoundObject(boundObject)
		, mShouldProvideThis(provideThis) { }

	void JSObjectEnvironment::CreateMutableBinding(JSStackFrame* exec, const UTF8String& name, mo_bool canDelete)
	{
		MO_ASSERT(!mBoundObject->HasProperty(exec, name));

		JSPropertyDescriptor desc;
		desc.Set(jsUndef(), (!canDelete ? JSPropertyAttributeDontDelete : JSPropertyAttributeNone));
		mBoundObject->DefineOwnProperty(exec, name, desc, mo_true);
	}

	void JSObjectEnvironment::SetMutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool strictMode)
	{
		JSPutPropertySlot slot(strictMode);
		mBoundObject->PutProperty(name, value, slot);
	}

	mo_bool JSObjectEnvironment::HasBinding(JSStackFrame* exec, const UTF8String& name)
	{
		return mBoundObject->HasProperty(exec, name);
	}

	mo_bool JSObjectEnvironment::DeleteBinding(JSStackFrame* exec, const UTF8String& name)
	{
		JSPropertyDescriptor desc;

		if(!mBoundObject->GetPropertyDescriptor(exec, name, desc))
			return mo_false;

		if(desc.IsConfigurable())
			return mBoundObject->DeleteProperty(exec, name, mo_true);

		return mo_false;
	}

	JSValue JSObjectEnvironment::GetBindingValue(JSStackFrame* exec, const UTF8String& name, mo_bool strictMode)
	{
		if(!mBoundObject->HasProperty(exec, name))
		{
			if(strictMode)
				jsThrowNativeError(kJSReferenceError);

			return jsUndef();
		}

		return mBoundObject->Get(exec, name);
	}

	JSValue JSObjectEnvironment::GetImplicitThisValue(JSStackFrame* exec)
	{
		if(mShouldProvideThis)
			return JSValue(mBoundObject);

		return jsUndef();
	}

	JSObjectEnvironment* JSObjectEnvironment::Create(JSGlobalObject* globalObject, JSObject* boundObject, mo_bool provideThis, JSExecutionEnvironment* parent)
	{
		return new JSObjectEnvironment(globalObject, boundObject, provideThis, parent);
	}
}