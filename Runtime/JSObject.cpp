#include "Runtime/JSObject.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSExecutionEnvironment.h"
#include "Runtime/JSFunction.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSPropertyMap.h"
#include "Runtime/JSPropertyNameList.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_BASE_CLASS(JSObject, Object);

	JSObject::JSObject(JSGlobalObject* globalObject)
		: mGlobalObject(globalObject)
		, mPrototype(jsNull())
		, mIsExtensible(mo_true)
		, mClassInfo(&JSObject::ClassInfo)
	{
		Initialize(JSObjectTypeUnknown);
	}

	JSObject::JSObject(JSGlobalObject* globalObject, JSObjectType type, JSValue prototype)
		: mGlobalObject(globalObject)
		, mPrototype(prototype)
		, mIsExtensible(mo_true)
		, mClassInfo(&JSObject::ClassInfo)
	{
		Initialize(type);
	}

	JSObject::JSObject(JSGlobalObject* globalObject, JSObjectType type, JSValue prototype, const JSClassInfo* classInfo)
		: mGlobalObject(globalObject)
		, mPrototype(prototype)
		, mIsExtensible(mo_true)
		, mClassInfo(classInfo)
	{
		Initialize(type);
	}

	void JSObject::Initialize(JSObjectType type)
	{
		mType = type;
		mProperties = new JSPropertyMap();
	}

	const JSClassFunctionTable* JSObject::GetClassTable() const
	{
		return &mClassInfo->Functions;
	}

	UTF8String JSObject::GetClassName() const
	{
		return GetClassTable()->GetClassNameImpl(this);
	}

	mo_bool JSObject::IsEnvironmentObject() const
	{
		return (mType == JSObjectTypeEnvironment);
	}

	mo_bool JSObject::IsGlobalObject() const
	{
		return (mType == JSObjectTypeGlobal);
	}

	mo_bool JSObject::IsString() const
	{
		return (mType == JSObjectTypeString);
	}

	mo_bool JSObject::IsObject() const
	{
		return (mType >= JSObjectTypeObject);
	}

	mo_bool JSObject::IsFunction() const
	{
		return (mType == JSObjectTypeFunction);
	}

	mo_bool JSObject::IsGetterOrSetter() const
	{
		return (mType == JSObjectTypeGetterSetter);
	}

	mo_bool JSObject::IsCallable() const
	{
		return GetClassTable()->IsCallableImpl();
	}

	mo_bool JSObject::ImplementsHasInstance() const
	{
		return GetClassTable()->ImplementsHasInstanceImpl();
	}

	mo_bool JSObject::IsSubClassOf(const JSClassInfo* info) const
	{
		return jsIsSubClass(mClassInfo, info);
	}

	mo_bool JSObject::IsPropertyEnumerable(const UTF8String& propertyName) const
	{
		return mo_false;
	}

	UTF8String JSObject::ResolveFunctionName(JSStackFrame* exec)
	{
		if(JSFunction* function = JSConvert::NullableCast<JSFunction*>(this))
			return function->GetDebuggerName(exec);
		if(JSConstructorFunction* function = JSConvert::NullableCast<JSConstructorFunction*>(this))
			return function->GetDebuggerName(exec);

		return UTF8String::MakeEmpty();
	}

	mo_bool JSObject::DefineOwnProperty(JSStackFrame* exec, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException)
	{
		return GetClassTable()->DefineOwnPropertyImpl(exec, this, propertyName, descriptor, throwException);
	}

	JSCallInfo JSObject::GetCallInfo(JSStackFrame* exec)
	{
		return GetClassTable()->GetCallInfoImpl(exec, this);
	}

	JSCallInfo JSObject::GetConstructInfo(JSStackFrame* exec)
	{
		return GetClassTable()->GetConstructInfoImpl(exec, this);
	}

	JSValue JSObject::GetDefaultValue(JSStackFrame* exec, JSTypePreference typeHint)
	{
		return GetClassTable()->GetDefaultValueImpl(exec, this, typeHint);
	}

	JSValue JSObject::GetValue(const UTF8String& propertyName) const
	{
		const JSPropertyMapEntry* entry = GetProperties()->Get(propertyName);

		if(!entry)
			return JSValue::MakeEmpty();

		return entry->value.Get();
	}

	JSValue JSObject::Get(JSStackFrame* exec, const UTF8String& propertyName) const
	{
		JSPropertySlot slot(this);

		if(const_cast<JSObject*>(this)->GetPropertySlot(exec, propertyName, slot))
			return slot.GetValue(exec, propertyName);

		return jsUndef();
	}

	JSValue JSObject::Get(JSStackFrame* exec, mo_unsigned propertyName) const
	{
		JSPropertySlot slot(this);

		if(const_cast<JSObject*>(this)->GetPropertySlot(exec, propertyName, slot))
			return slot.GetValue(exec, propertyName);

		return jsUndef();
	}

	void JSObject::GetOwnPropertyNames(JSStackFrame* exec, JSPropertyNameList& namesList, mo_bool includeDontEnumProps)
	{
		if(GetClassTable()->GetOwnPropertyNamesImpl)
			GetClassTable()->GetOwnPropertyNamesImpl(exec, this, namesList, includeDontEnumProps);
	}

	void JSObject::GetPropertyNames(JSStackFrame* exec, JSPropertyNameList& namesList, mo_bool includeDontEnumProps)
	{
		if(GetClassTable()->GetPropertyNamesImpl)
			GetClassTable()->GetPropertyNamesImpl(exec, this, namesList, includeDontEnumProps);
	}

	mo_bool JSObject::GetPropertySlot(JSStackFrame* exec, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		JSObject* obj = this;

		while(mo_true)
		{
			if(obj->GetClassTable()->GetOwnPropertySlotImpl(exec, obj, propertyName, slot))
				return mo_true;

			JSValue prototype = obj->GetPrototype();

			if(!prototype.IsObject())
				return mo_false;

			obj = prototype.ToNativeObject();
		}

		return mo_false;
	}

	mo_bool JSObject::GetPropertySlot(JSStackFrame* exec, mo_unsigned propertyName, JSPropertySlot& slot)
	{
		JSObject* obj = this;

		while(mo_true)
		{
			if(obj->GetClassTable()->GetOwnPropertySlotByIndexImpl(exec, obj, propertyName, slot))
				return mo_true;

			JSValue prototype = obj->GetPrototype();

			if(!prototype.IsObject())
				return mo_false;

			obj = prototype.ToNativeObject();
		}

		return mo_false;
	}

	mo_bool JSObject::GetPropertyDescriptor(JSStackFrame* exec, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		JSObject* obj = this;

		while(mo_true)
		{
			if(obj->GetClassTable()->GetOwnPropertyDescriptorImpl(exec, obj, propertyName, descriptor))
				return mo_true;

			JSValue prototype = obj->GetPrototype();

			if(!prototype.IsObject())
				return mo_false;

			obj = prototype.ToNativeObject();
		}

		return mo_false;
	}

	mo_bool JSObject::GetOwnPropertyDescriptor(JSStackFrame* exec, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		return GetClassTable()->GetOwnPropertyDescriptorImpl(exec, this, propertyName, descriptor);
	}

	mo_bool JSObject::GetOwnPropertySlot(JSStackFrame* exec, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		return GetClassTable()->GetOwnPropertySlotImpl(exec, this, propertyName, slot);
	}

	mo_bool JSObject::GetOwnPropertySlotByIndex(JSStackFrame* exec, mo_unsigned propertyName, JSPropertySlot& slot)
	{
		return GetClassTable()->GetOwnPropertySlotByIndexImpl(exec, this, propertyName, slot);
	}

	void JSObject::PutProperty(const UTF8String& propertyName, JSValue value, mo_unsigned attributes)
	{
		MO_ASSERT(!value.IsGetterOrSetter() && (attributes & JSPropertyAttributeAccessor) == 0);

		JSPutPropertySlot slot;
		PutProperty(propertyName, value, attributes, slot, mo_false);
	}

	void JSObject::PutProperty(const UTF8String& propertyName, JSValue value, JSPutPropertySlot& slot)
	{
		MO_ASSERT(!value.IsGetterOrSetter());
		PutProperty(propertyName, value, 0, slot, mo_false);
	}

	void JSObject::PutPropertyAccessor(const UTF8String& propertyName, JSValue value, mo_unsigned attributes)
	{
		MO_ASSERT(value.IsGetterOrSetter() && (attributes & JSPropertyAttributeAccessor) != 0);

		JSPutPropertySlot slot;
		PutProperty(propertyName, value, attributes, slot, mo_false);
	}

	void JSObject::Put(JSStackFrame* exec, const UTF8String& propertyName, JSValue value, mo_unsigned attributes)
	{
		GetClassTable()->PutImpl(exec, this, propertyName, value, attributes);
	}

	void JSObject::PutProperty(JSStackFrame* exec, const UTF8String& propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot)
	{
		GetClassTable()->PutPropertyImpl(exec, this, propertyName, value, throwException, slot);
	}

	void JSObject::PutPropertyByIndex(JSStackFrame* exec, mo_unsigned propertyName, JSValue value, mo_bool throwException)
	{
		GetClassTable()->PutPropertyByIndexImpl(exec, this, propertyName, value, throwException);
	}

	mo_bool JSObject::DeleteProperty(JSStackFrame* exec, const UTF8String& propertyName, mo_bool throwException, mo_bool force)
	{
		return GetClassTable()->DeletePropertyImpl(exec, this, propertyName, throwException, force);
	}

	mo_bool JSObject::DeletePropertyByIndex(JSStackFrame* exec, mo_unsigned propertyName, mo_bool throwException, mo_bool force)
	{
		return GetClassTable()->DeletePropertyByIndexImpl(exec, this, propertyName, throwException, force);
	}

	mo_bool JSObject::HasProperty(JSStackFrame* exec, const UTF8String& propertyName) const
	{
		JSPropertySlot slot;
		return const_cast<JSObject*>(this)->GetPropertySlot(exec, propertyName, slot);
	}

	mo_bool JSObject::HasProperty(JSStackFrame* exec, mo_unsigned propertyName) const
	{
		JSPropertySlot slot;
		return const_cast<JSObject*>(this)->GetPropertySlot(exec, propertyName, slot);
	}

	mo_bool JSObject::HasOwnProperty(JSStackFrame* exec, const UTF8String& propertyName) const
	{
		JSPropertySlot slot;
		return const_cast<JSObject*>(this)->GetOwnPropertySlot(exec, propertyName, slot);
		//return const_cast<JSObject*>(this)->GetClassTable()->GetOwnPropertySlotImpl(exec, const_cast<JSObject*>(this), propertyName, slot);
	}

	mo_bool JSObject::HasInstance(JSStackFrame* exec, JSValue value, JSValue prototype)
	{
		return GetClassTable()->HasInstanceImpl(exec, this, value, prototype);
	}

	mo_bool JSObject::DeleteProperty(const UTF8String& propertyName)
	{
		return mProperties->Remove(propertyName);
	}

	mo_bool JSObject::DeleteProperty(mo_unsigned propertyName)
	{
		return DeleteProperty(UTF8String::FromNumber(propertyName));
	}

	mo_bool JSObject::PutProperty(const UTF8String& propertyName, JSValue value, mo_unsigned attributes, JSPutPropertySlot& slot, mo_bool honorAttributes)
	{
		MO_ASSERT(value);
		MO_ASSERT(value.IsGetterOrSetter() == !!(attributes & JSPropertyAttributeAccessor));

		const JSPropertyMapEntry* entry = mProperties->Get(propertyName);

		if(entry)
		{
			if(honorAttributes && (entry->attributes & JSPropertyAttributeReadOnly) != 0)
				return mo_false;

			mProperties->Set(propertyName, value, attributes);

			slot.SetExisting(this);
			return mo_true;
		}

		if(honorAttributes && !IsExtensible())
			return mo_false;

		mProperties->Set(propertyName, value, attributes);

		slot.SetNew(this);
		return mo_true;
	}

	mo_bool JSObject::ToBoolean(JSStackFrame* exec) const
	{
		if(IsString())
			return mo_cast<const JSString*>(this)->ToBoolean(exec);

		return mo_true;
	}

	mo_double JSObject::ToNumber(JSStackFrame* exec) const
	{
		if(IsString())
			return mo_cast<const JSString*>(this)->ToNumber(exec);

		JSValue primitiveValue = ToPrimitive(exec, JSTypePreferNumber);

		if(exec->HasException())
			return 0.0;

		return primitiveValue.ToNumber(exec);
	}

	JSString* JSObject::ToString(JSStackFrame* exec) const
	{
		if(IsString())
			return mo_cast_const<JSString*>(mo_cast<const JSString*>(this));

		JSValue primitive = ToPrimitive(exec, JSTypePreferString);

		if(exec->HasException())
			return jsEmptyString(mGlobalObject);

		return primitive.ToString(exec);
	}

	JSObject* JSObject::ToThisObject(JSStackFrame* exec)
	{
		return GetClassTable()->ToThisObjectImpl(exec, this);
	}

	JSObject* JSObject::ToObject(JSStackFrame* exec)
	{
		if(IsString())
			return mo_cast<const JSString*>(this)->ToObject(exec);

		return (IsObject() ? this : mo_null);
	}

	JSValue JSObject::ToPrimitive(JSStackFrame* exec, JSTypePreference preference) const
	{
		if(IsString())
			return mo_cast<const JSString*>(this);

		return GetClassTable()->GetDefaultValueImpl(exec, this, preference);
	}

	JSValue JSObject::ToPrimitiveNumber(JSStackFrame* exec, mo_double& number) const
	{
		if(IsString())
			return mo_cast<const JSString*>(this)->ToPrimitiveNumber(exec, number);

		JSValue result = GetClassTable()->GetDefaultValueImpl(exec, this, JSTypePreferNumber);
		number = result.ToNumber(exec);

		return result;
	}
}