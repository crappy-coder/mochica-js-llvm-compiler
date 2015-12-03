#include "Runtime/JSUnit.h"
#include "Runtime/JSExecutionEnvironment.h"
#include "Runtime/JSObject.h"

namespace MoEnjin
{
	JSUnit::JSUnit(JSObject* prototype, const JSClassInfo* classInfo)
		: mClassInfo(classInfo)
		, mPrototype(prototype)
	{
		//MO_ASSERT(mClassInfo);
		//MO_ASSERT(mPrototype);

		//MO_ASSERT(mPrototype.IsObject() || mPrototype.IsNull());
	}

	mo_bool JSUnit::IsSubClassOf(const JSClassInfo* info) const
	{
		return jsIsSubClass(mClassInfo, info);
	}

	mo_bool JSUnit::IsEnvironmentObject() const
	{
		return jsIsSubClass(mClassInfo, &JSExecutionEnvironment::ClassInfo);
	}

	const JSClassFunctionTable* JSUnit::GetClassTable() const
	{
		return &mClassInfo->Functions;
	}

	mo_bool JSUnit::ToBoolean(JSStackFrame* exec) const
	{
		return mo_false;
	}

	mo_double JSUnit::ToNumber(JSStackFrame* exec) const
	{
		return 0.0;
	}

	mo_bool JSUnit::ToString(JSStackFrame* exec, UTF8String& str) const
	{
		return mo_false;
	}

	UTF8String JSUnit::ToString(JSStackFrame* exec) const
	{
		return UTF8String();
	}

	JSObject* JSUnit::ToObject(JSStackFrame* exec) const
	{
		return 0;
	}

	JSObject* JSUnit::ToObjectRef(JSStackFrame* exec) const
	{
		return 0;
	}

	JSValue JSUnit::ToPrimitive(JSStackFrame* exec, JSTypePreference preference) const
	{
		return JSValue();
	}

	JSValue JSUnit::ToPrimitiveNumber(JSStackFrame* exec, mo_double& number) const
	{
		return JSValue();
	}

	mo_bool JSUnit::IsString() const
	{
		return mo_false;
	}

	mo_bool JSUnit::IsObject() const
	{
		return mo_false;
	}

	mo_bool JSUnit::IsGetterOrSetter() const
	{
		return mo_false;
	}

	UTF8String JSUnit::GetClassNameImpl(const JSObject* obj)
	{
		return UTF8String();
	}

	mo_bool JSUnit::IsExtensibleImpl(const JSObject* obj)
	{
		return mo_false;
	}

	mo_bool JSUnit::GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		MO_ASSERT_NOT_REACHED();
		return mo_false;
	}

	mo_bool JSUnit::GetOwnPropertySlotImpl(JSStackFrame* exec, JSUnit* unit, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		JSObject* obj = unit->ToObject(exec);
		slot.SetBase(obj);

		if(!obj->GetPropertySlot(exec, propertyName, slot))
			slot.SetUndefined();

		return mo_true;
	}

	mo_bool JSUnit::GetOwnPropertySlotByIndexImpl(JSStackFrame* exec, JSUnit* unit, mo_unsigned propertyName, JSPropertySlot& slot)
	{
		JSObject* obj = unit->ToObject(exec);
		slot.SetBase(obj);

		if(!obj->GetPropertySlot(exec, propertyName, slot))
			slot.SetUndefined();

		return mo_true;
	}

	void JSUnit::PutImpl(JSStackFrame* exec, JSUnit* unit, const UTF8String& propertyName, JSValue value, mo_unsigned attributes)
	{
		MO_ASSERT_NOT_REACHED();
	}

	void JSUnit::PutPropertyImpl(JSStackFrame* exec, JSUnit* unit, const UTF8String& propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot)
	{
		if(unit->IsString())
		{
			// TODO
			//JSValue(unit).PutToPrimitive(exec, propertyName, value, slot);
			return;
		}

		JSObject* thisObj = unit->ToObject(exec);
		thisObj->GetClassTable()->PutPropertyImpl(exec, thisObj, propertyName, value, throwException, slot);
	}

	void JSUnit::PutPropertyByIndexImpl(JSStackFrame* exec, JSUnit* unit, mo_unsigned propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot)
	{
		if(unit->IsString())
		{
			// TODO
			//JSValue(unit).PutToPrimitive(exec, UTF8String::FromNumber(propertyName), value, slot);
			return;
		}

		JSObject* thisObj = unit->ToObject(exec);
		thisObj->GetClassTable()->PutPropertyByIndexImpl(exec, thisObj, propertyName, value, throwException, slot);
	}

	mo_bool JSUnit::DeletePropertyImpl(JSStackFrame* exec, JSUnit* unit, const UTF8String& propertyName, mo_bool throwException)
	{
		JSObject* thisObj = unit->ToObject(exec);
		return thisObj->GetClassTable()->DeletePropertyImpl(exec, thisObj, propertyName, throwException);
	}

	mo_bool JSUnit::DeletePropertyByIndexImpl(JSStackFrame* exec, JSUnit* unit, mo_unsigned propertyName, mo_bool throwException)
	{
		JSObject* thisObj = unit->ToObject(exec);
		return thisObj->GetClassTable()->DeletePropertyByIndexImpl(exec, thisObj, propertyName, throwException);
	}

	mo_bool JSUnit::DefineOwnPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException)
	{
		MO_ASSERT_NOT_REACHED();
		return mo_false;
	}

	JSValue JSUnit::GetDefaultValueImpl(JSStackFrame* exec, const JSObject* obj, JSTypePreference typeHint)
	{
		MO_ASSERT_NOT_REACHED();
		return jsUndef();
	}

	JSObject* JSUnit::ToThisObjectImpl(JSStackFrame* exec, JSUnit* unit)
	{
		return unit->ToObject(exec);
	}

	mo_bool JSUnit::HasInstanceImpl(JSStackFrame* exec, JSObject* obj, JSValue value, JSValue prototype)
	{
		MO_ASSERT_NOT_REACHED();
		return mo_false;
	}

	NativeFunction JSUnit::GetCallFunctionImpl(JSUnit* unit)
	{
		return mo_null;
	}

	NativeFunction JSUnit::GetConstructFunctionImpl(JSUnit* unit)
	{
		return mo_null;
	}
}