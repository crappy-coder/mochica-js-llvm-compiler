#include "Runtime/JSObjectPrototype.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	static JSValueImpl JSObjectPrototype_ToString(JSStackFrame*);
	static JSValueImpl JSObjectPrototype_ToLocaleString(JSStackFrame*);
	static JSValueImpl JSObjectPrototype_ValueOf(JSStackFrame*);
	static JSValueImpl JSObjectPrototype_HasOwnProperty(JSStackFrame*);
	static JSValueImpl JSObjectPrototype_IsPrototypeOf(JSStackFrame*);
	static JSValueImpl JSObjectPrototype_PropertyIsEnumerable(JSStackFrame*);
}

#include "Runtime/Tables/JSObjectPrototypeTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSObjectPrototype, Object, JSObject);

	JSObjectPrototype::JSObjectPrototype(JSGlobalObject* globalObject)
		: JSObject(globalObject, JSObjectTypeObject, jsNull(), &ClassInfo)
		, mHasIntegerPropertyNames(mo_false) { }

	JSObjectPrototype* JSObjectPrototype::Create(JSGlobalObject* globalObject)
	{
		return new JSObjectPrototype(globalObject);
	}

	mo_bool JSObjectPrototype::GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		return GetStaticFunctionDescriptor<JSObject>(exec, &JSObjectPrototypeTable, JSConvert::Cast<JSObjectPrototype*>(obj), propertyName, descriptor);
	}

	mo_bool JSObjectPrototype::GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		return GetStaticFunctionSlot<JSObject>(exec, &JSObjectPrototypeTable, JSConvert::Cast<JSObjectPrototype*>(obj), propertyName, slot);
	}

	mo_bool JSObjectPrototype::GetOwnPropertySlotByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, JSPropertySlot& slot)
	{
		JSObjectPrototype* me = JSConvert::Cast<JSObjectPrototype*>(obj);

		if(!me->mHasIntegerPropertyNames)
			return mo_false;

		return JSObject::GetOwnPropertySlotByIndexImpl(exec, obj, propertyName, slot);
	}

	mo_bool JSObjectPrototype::DefineOwnPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException)
	{
		JSObjectPrototype* me = JSConvert::Cast<JSObjectPrototype*>(obj);

		if(!me->mHasIntegerPropertyNames)
		{
			mo_uint32 n = 0;

			if(propertyName.ToUInt32(n))
				me->mHasIntegerPropertyNames = mo_true;
		}

		return JSObject::DefineOwnPropertyImpl(exec, obj, propertyName, descriptor, throwException);
	}

	void JSObjectPrototype::PutPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot)
	{
		JSObjectPrototype* me = JSConvert::Cast<JSObjectPrototype*>(obj);

		if(!me->mHasIntegerPropertyNames)
		{
			mo_uint32 n = 0;

			if(propertyName.ToUInt32(n))
				me->mHasIntegerPropertyNames = mo_true;
		}

		return JSObject::PutPropertyImpl(exec, obj, propertyName, value, throwException, slot);
	}

	JSValueImpl JSObjectPrototype_ToString(JSStackFrame* exec)
	{
		JSValue thisValue = exec->GetThisValue();
		JSObject* thisObject = mo_null;
		JSString* result = mo_null;

		if(thisValue.IsNullOrUndefined())
			result = JSString::FromString(exec->GetGlobalObject(), thisValue.IsUndefined() ? "[object Undefined]" : "[object Null]");
		else
		{
			thisObject = thisValue.ToObject(exec);
			result = JSString::FromString(exec->GetGlobalObject(), "[object " + thisObject->GetClassName() + "]");
		}

		return JSValue::Encode(result);
	}

	JSValueImpl JSObjectPrototype_ToLocaleString(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSObjectPrototype_ValueOf(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSObjectPrototype_HasOwnProperty(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSObjectPrototype_IsPrototypeOf(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSObjectPrototype_PropertyIsEnumerable(JSStackFrame* exec)
	{
		return jsUndef();
	}
}