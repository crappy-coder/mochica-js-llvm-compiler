#include "Runtime/JSScriptEnvironment.h"

namespace MoEnjin
{
	JSScriptEnvironment::JSScriptEnvironment(JSGlobalObject* globalObject, JSExecutionEnvironment* parent)
		: JSExecutionEnvironment(globalObject, parent) { }

	void JSScriptEnvironment::CreateImmutableBinding(JSStackFrame* exec, const UTF8String& name)
	{
		MO_ASSERT(!HasBinding(exec, name));

		JSPropertyDescriptor desc;
		desc.Set(jsNull(), JSPropertyAttributeNone);
		DefineOwnProperty(exec, name, desc, mo_true);
	}

	void JSScriptEnvironment::InitializeImmutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value)
	{
		JSPropertyDescriptor desc;
		
		MO_ASSERT(GetPropertyDescriptor(exec, name, desc) && desc.GetValue().IsNull());
		
		Put(exec, name, value, JSPropertyAttributeDontDelete | JSPropertyAttributeDontEnum | JSPropertyAttributeReadOnly);
	}

	void JSScriptEnvironment::CreateMutableBinding(JSStackFrame* exec, const UTF8String& name, mo_bool canDelete)
	{
		MO_ASSERT(!HasProperty(exec, name));

		JSPropertyDescriptor desc;
		desc.Set(jsUndef(), (canDelete ? JSPropertyAttributeNone : JSPropertyAttributeDontDelete));
		DefineOwnProperty(exec, name, desc, mo_true);
	}

	void JSScriptEnvironment::SetMutableBinding(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool strictMode)
	{
		JSPropertyDescriptor desc;

		MO_ASSERT(GetPropertyDescriptor(exec, name, desc));

		if(!desc.IsWritable())
		{
			jsThrowNativeError(kJSTypeError);
			return;
		}

		JSPutPropertySlot slot(strictMode);
		PutProperty(name, value, slot);
	}

	mo_bool JSScriptEnvironment::HasBinding(JSStackFrame* exec, const UTF8String& name)
	{
		return HasProperty(exec, name);
	}

	mo_bool JSScriptEnvironment::DeleteBinding(JSStackFrame* exec, const UTF8String& name)
	{
		JSPropertyDescriptor desc;

		if(!GetPropertyDescriptor(exec, name, desc))
			return mo_true;

		if(desc.IsConfigurable())
			return DeleteProperty(exec, name, mo_true);

		return mo_false;
	}

	JSValue JSScriptEnvironment::GetBindingValue(JSStackFrame* exec, const UTF8String& name, mo_bool strictMode)
	{
		JSPropertyDescriptor desc;

		MO_ASSERT(GetPropertyDescriptor(exec, name, desc));

		JSValue value = desc.GetValue();

		if(value.IsNull())
		{
			if(strictMode)
				jsThrowNativeError(kJSReferenceError);

			return jsUndef();
		}

		return value;
	}

	JSValue JSScriptEnvironment::GetImplicitThisValue(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSScriptEnvironment* JSScriptEnvironment::Create(JSGlobalObject* globalObject, JSExecutionEnvironment* parent)
	{
		return new JSScriptEnvironment(globalObject, parent);
	}
}