#include "Runtime/JSObject.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSGetterSetter.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSObjectPrototype.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSPropertyMap.h"
#include "Runtime/JSPropertyNameList.h"
#include "Runtime/JSValueWrapper.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	static mo_bool PutDescriptor(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_unsigned attributes, const JSPropertyDescriptor& prevDescriptor);
	static JSValue GetDefaultValueFromFuncCall(JSStackFrame* exec, const JSObject* obj, const UTF8String& propertyName);

	JSObject* JSObject::Create(JSStackFrame* exec)
	{
		return JSObject::Create(exec, exec->GetGlobalObject()->GetObjectPrototype());
	}

	JSObject* JSObject::Create(JSStackFrame* exec, JSValue prototype)
	{
		return new JSObject(exec->GetGlobalObject(), JSObjectTypeObject, prototype);
	}

	UTF8String JSObject::GetClassNameImpl(const JSObject* obj)
	{
		return obj->GetClassInfo()->Name;
	}

	mo_bool JSObject::IsExtensibleImpl(const JSObject* obj)
	{
		return obj->IsExtensible();
	}

	mo_bool JSObject::IsCallableImpl()
	{
		return mo_false;
	}

	mo_bool JSObject::ImplementsHasInstanceImpl()
	{
		return mo_false;
	}

	void JSObject::GetOwnPropertyNamesImpl(JSStackFrame* exec, JSObject* obj, JSPropertyNameList& namesList, mo_bool includeDontEnumProps)
	{
		// first, gather all of the dynamic properties of obj and add them 
		// to the list of names, if the list does not yet contain any names 
		// then we know when we add a new one it will be unique and can 
		// optimize for it
		mo_bool skipDuplicateCheck = (namesList.GetCount() == 0);

		for(JSPropertyMap::const_iterator i = obj->GetProperties()->GetTable().begin(), e = obj->GetProperties()->GetTable().end(); i != e; ++i)
		{
			const JSPropertyMapEntry& entry = i->getValue();

			if((entry.attributes & JSPropertyAttributeDontEnum) == 0 || includeDontEnumProps)
				namesList.Add(*entry.name, skipDuplicateCheck);
		}


		// second, if the class has any static properties then add these
		// to the names list
		for(const JSClassInfo* classInfo = obj->GetClassInfo(); classInfo; classInfo = classInfo->Parent)
		{
			const JSClassPropertyTable* table = classInfo->Properties;

			// the class has no static properties
			if(!table)
				continue;

			// make sure the table has been initialized with the
			// properties
			table->EnsureInitialized();

			for(JSClassPropertyTable::const_iterator i = table->mTable.begin(), e = table->mTable.end(); i != e; ++i)
			{
				UTF8String key = UTF8String(i->getKeyData(), i->getKeyLength());
				const JSClassPropertyTableEntry& entry = i->getValue();

				if(!key.IsNull() && (((entry.GetAttributes() & JSPropertyAttributeDontEnum) == 0) || includeDontEnumProps))
					namesList.Add(key);
			}
		}
	}

	void JSObject::GetPropertyNamesImpl(JSStackFrame* exec, JSObject* obj, JSPropertyNameList& namesList, mo_bool includeDontEnumProps)
	{
		// get own properties first
		obj->GetOwnPropertyNames(exec, namesList, includeDontEnumProps);

		// object has no prototype so we are done here
		if(!obj->HasPrototype())
			return;


		// walk the prototype chain adding each unique property
		// name to our names list
		JSObject* prototype = obj->GetPrototype().ToNativeObject();

		while(mo_true)
		{
			if(JS_OBJECT_OVERRIDES_FUNCTION(prototype, GetPropertyNamesImpl))
			{
				prototype->GetPropertyNames(exec, namesList, includeDontEnumProps);
				break;
			}

			prototype->GetOwnPropertyNames(exec, namesList, includeDontEnumProps);

			if(!prototype->HasPrototype())
				break;

			prototype = prototype->GetPrototype().ToNativeObject();
		}
	}

	mo_bool JSObject::GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		const JSPropertyMapEntry* entry = obj->GetProperties()->Get(propertyName);

		if(!entry)
			return mo_false;

		descriptor.Set(entry->value.Get(), entry->attributes);
		return mo_true;
	}

	mo_bool JSObject::GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		if(const JSValueWrapper<JSAnyValue>* value = obj->GetProperties()->GetDirectValue(propertyName))
		{
			if(!value->IsGetterSetter())
				slot.SetValue(obj, value->Get());
			else
			{
				JSObject* getterFunc = JSConvert::ToGetterSetter(value->Get())->GetGetter();
				
				if(getterFunc)
					slot.SetGetter(getterFunc);
				else
					slot.SetUndefined();
			}

			return mo_true;
		}

		return mo_false;
	}

	mo_bool JSObject::GetOwnPropertySlotByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, JSPropertySlot& slot)
	{
		return obj->GetClassTable()->GetOwnPropertySlotImpl(exec, obj, UTF8String::FromNumber(propertyName), slot);
	}

	void JSObject::PutImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSValue value, mo_unsigned attributes)
	{
		MO_ASSERT(!value.IsGetterOrSetter() && (attributes & JSPropertyAttributeAccessor) == 0);

		JSPutPropertySlot slot;
		obj->PutProperty(propertyName, value, attributes, slot);
	}

	void JSObject::PutPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot)
	{
		MO_ASSERT(value);

		JSGlobalObject* globalObj = exec->GetGlobalObject();
		JSValue prototype;

		for(JSObject* objProto = obj; ; objProto = prototype.ToNativeObject())
		{
			const JSPropertyMapEntry* entry = objProto->GetProperties()->Get(propertyName);

			if(entry)
			{
				if(entry->attributes & JSPropertyAttributeReadOnly)
				{
					if(slot.IsStrictMode())
						jsThrowNativeError(kJSTypeError); // cannot write to readonly property in strict mode

					return;
				}

				if(entry->value.IsGetterSetter())
				{
					JSObject* setterFunc = JSConvert::ToGetterSetter(entry->value.Get())->GetSetter();

					if(!setterFunc)
					{
						if(slot.IsStrictMode())
							jsThrowNativeError(kJSTypeError); // writing to a property that only has a getter

						return;
					}

					exec->ExecuteCall(setterFunc, setterFunc->GetCallInfo(exec), obj, JSArgList(value)); 
					return;
				}

				break;
			}

			prototype = objProto->GetPrototype();

			if(prototype.IsNull())
				break;
		}

		if(!obj->PutProperty(propertyName, value, 0, slot) && slot.IsStrictMode())
			jsThrowNativeError(kJSTypeError);
	}

	void JSObject::PutPropertyByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, JSValue value, mo_bool throwException)
	{
		JSPutPropertySlot slot(throwException);
		obj->GetClassTable()->PutPropertyImpl(exec, obj, UTF8String::FromNumber(propertyName), value, throwException, slot);
	}

	mo_bool JSObject::DeletePropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, mo_bool throwException, mo_bool force)
	{
		const JSPropertyMapEntry* entry = obj->GetProperties()->Get(propertyName);

		if(entry)
		{
			if((entry->attributes & JSPropertyAttributeDontDelete) != 0 && !force)
				return mo_false;

			obj->GetProperties()->Remove(propertyName);
			return mo_true;
		}

		// TODO - allow for static property deletions?
		return mo_true;
	}

	mo_bool JSObject::DeletePropertyByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, mo_bool throwException, mo_bool force)
	{
		return obj->GetClassTable()->DeletePropertyImpl(exec, obj, UTF8String::FromNumber(propertyName), throwException, force);
	}

	mo_bool JSObject::DefineOwnPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException)
	{
		JSPropertyMap* props = obj->GetProperties();
		JSPropertyDescriptor current;

		// this is a new property, if the object is extensible then we can
		// just put the value normally
		if(!obj->GetClassTable()->GetOwnPropertyDescriptorImpl(exec, obj, propertyName, current))
		{
			if(!obj->IsExtensible())
			{
				if(throwException)
					jsThrowNativeError(kJSTypeError); // cannot define a property on an object that is not extensible

				return mo_false;
			}

			JSPropertyDescriptor prevDesc;
			prevDesc.SetValue(jsUndef());
			return PutDescriptor(exec, obj, propertyName, descriptor, descriptor.GetAttributes(), prevDesc);	
		}


		// nothing to do
		if(descriptor.IsEmpty() || current.IsEqualTo(descriptor))
			return mo_true;


		// validate configuration changes on an unconfigurable property
		if(!current.IsConfigurable())
		{
			if(descriptor.IsConfigurable())
			{
				if(throwException)
					jsThrowNativeError(kJSTypeError); // cannot enable the configurable attribute of unconfigurable property

				return mo_false;
			}

			if(descriptor.IsEnumerableSet() && descriptor.IsEnumerable() != current.IsEnumerable())
			{
				if(throwException)
					jsThrowNativeError(kJSTypeError); // cannot change enumerable attribute of unconfigurable property

				return mo_false;
			}
		}


		// generic descriptors just change attributes of the existing property
		if(descriptor.IsGeneric())
		{
			if(!current.HasEqualAttributes(descriptor))
			{
				obj->DeleteProperty(propertyName);
				return PutDescriptor(exec, obj, propertyName, descriptor, descriptor.GetOverridingAttributes(current), current);
			}

			return mo_true;
		}


		// changing between a normal or an accessor property
		if(descriptor.IsData() != current.IsData())
		{
			if(!current.IsConfigurable())
			{
				if(throwException)
					jsThrowNativeError(kJSTypeError); // cannot change data access for an unconfigurable property

				return mo_false;
			}

			obj->DeleteProperty(propertyName);
			return PutDescriptor(exec, obj, propertyName, descriptor, descriptor.GetOverridingAttributes(current), current);
		}


		// changing both the value and attributes of an existing property
		if(descriptor.IsData())
		{
			if(!current.IsConfigurable())
			{
				if(!current.IsWritable())
				{
					if(descriptor.IsWritable())
					{
						if(throwException)
							jsThrowNativeError(kJSTypeError); // cannot change writable attribute of an unconfigurable property

						return mo_false;
					}

					if(descriptor.HasValue() && !JSValue::AreSameValue(current.GetValue(), descriptor.GetValue()))
					{
						if(throwException)
							jsThrowNativeError(kJSTypeError); // cannot change the value of a readonly property

						return mo_false;
					}
				}
			}

			if(current.HasEqualAttributes(descriptor) && !descriptor.HasValue())
				return mo_true;

			obj->DeleteProperty(propertyName);
			return PutDescriptor(exec, obj, propertyName, descriptor, descriptor.GetOverridingAttributes(current), current);
		}

		// must be an accessor property now
		MO_ASSERT(descriptor.IsAccessor());

		// changing the accessor functions of exsiting accessor property
		if(!current.IsConfigurable())
		{
			if(descriptor.HasSetter() && !(current.HasSetter() && JSValue::AreStrictEqual(current.GetSetterValue(), descriptor.GetSetterValue())))
			{
				if(throwException)
					jsThrowNativeError(kJSTypeError); // cannot change the setter of an unconfigurable property

				return mo_false;
			}

			if(descriptor.HasGetter() && !(current.HasGetter() && JSValue::AreStrictEqual(current.GetGetterValue(), descriptor.GetGetterValue())))
			{
				if(throwException)
					jsThrowNativeError(kJSTypeError); // cannot change the getter of an unconfigurable property

				return mo_false;
			}
		}

		// TODO - need to test this...
		JSValue accessor = obj->Get(exec, propertyName);

		if(!accessor)
			return mo_false;

		JSGetterSetter* getterSetter = JSConvert::ToGetterSetter(accessor);

		if(descriptor.HasSetter())
			getterSetter->SetSetter(descriptor.GetSetter());
		if(descriptor.HasGetter())
			getterSetter->SetGetter(descriptor.GetGetter());

		if(current.HasEqualAttributes(descriptor))
			return mo_true;

		obj->DeleteProperty(propertyName);
		obj->PutPropertyAccessor(propertyName, getterSetter, descriptor.GetOverridingAttributes(current) | JSPropertyAttributeAccessor);

		return mo_true;
	}

	JSValue JSObject::GetDefaultValueImpl(JSStackFrame* exec, const JSObject* obj, JSTypePreference typeHint)
	{	
		JSGlobalObject* globalObj = exec->GetGlobalObject();

		// toString needs to be called first for date objects
		if((typeHint == JSTypePreferString) || (typeHint != JSTypePreferNumber && obj->GetPrototype() == exec->GetGlobalObject()->GetDatePrototype()))
		{
			// try toString first...
			JSValue value = GetDefaultValueFromFuncCall(exec, obj, KnownIdentifiers::toString);

			// then valueOf...
			if(!value)
				value = GetDefaultValueFromFuncCall(exec, obj, KnownIdentifiers::valueOf);

			if(value)
				return value;
		}
		else
		{
			// try valueOf first this time...
			JSValue value = GetDefaultValueFromFuncCall(exec, obj, KnownIdentifiers::valueOf);

			// then toString...
			if(!value)
				value = GetDefaultValueFromFuncCall(exec, obj, KnownIdentifiers::toString);

			if(value)
				return value;
		}

		MO_ASSERT(!exec->GetGlobalObject()->HasException());

		jsThrowNativeErrorWithMessage(kJSTypeError, "No default value.");
		return jsUndef();
	}

	JSObject* JSObject::ToThisObjectImpl(JSStackFrame* exec, JSObject* obj)
	{
		return obj;
	}

	mo_bool JSObject::HasInstanceImpl(JSStackFrame* exec, JSObject* obj, JSValue value, JSValue prototype)
	{
		if(!value.IsObject())
			return mo_false;

		if(!prototype.IsObject())
		{
			jsThrowNativeError(kJSTypeError); // instanceof called on an object with an invalid prototype property
			return mo_false;
		}

		JSObject* valueObject = value.ToNativeObject();

		while((valueObject = valueObject->GetPrototype().ToNativeObject()))
		{
			if(prototype == valueObject)
				return mo_true;
		}

		return mo_false;
	}

	JSCallInfo JSObject::GetCallInfoImpl(JSStackFrame* exec, JSObject* obj)
	{
		return jsNoCallInfo();
	}

	JSCallInfo JSObject::GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj)
	{
		return jsNoCallInfo();
	}

	mo_bool PutDescriptor(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_unsigned attributes, const JSPropertyDescriptor& prevDescriptor)
	{
		if(descriptor.IsGeneric() ||descriptor.IsData())
		{
			if(descriptor.IsGeneric() && prevDescriptor.IsAccessor())
			{
				JSGetterSetter* accessor = new JSGetterSetter(exec->GetGlobalObject());

				if(prevDescriptor.HasGetter())
					accessor->SetGetter(prevDescriptor.GetGetter());
				if(prevDescriptor.HasSetter())
					accessor->SetSetter(prevDescriptor.GetSetter());

				obj->PutPropertyAccessor(propertyName, accessor, attributes | JSPropertyAttributeAccessor);
				return mo_true;
			}

			JSValue newValue = jsUndef();

			if(descriptor.HasValue())
				newValue = descriptor.GetValue();
			else if(prevDescriptor.HasValue())
				newValue = prevDescriptor.GetValue();

			obj->PutProperty(propertyName, newValue, attributes & ~JSPropertyAttributeAccessor);
			return mo_true;
		}

		attributes &= ~JSPropertyAttributeReadOnly;

		JSGetterSetter* accessor = new JSGetterSetter(exec->GetGlobalObject());

		if(descriptor.HasGetter())
			accessor->SetGetter(descriptor.GetGetter());
		else if(prevDescriptor.HasGetter())
			accessor->SetGetter(prevDescriptor.GetGetter());

		if(descriptor.HasSetter())
			accessor->SetSetter(descriptor.GetSetter());
		else if(prevDescriptor.HasSetter())
			accessor->SetSetter(prevDescriptor.GetSetter());

		obj->PutPropertyAccessor(propertyName, accessor, attributes | JSPropertyAttributeAccessor);
		return mo_true;
	}

	JSValue GetDefaultValueFromFuncCall(JSStackFrame* exec, const JSObject* obj, const UTF8String& propertyName)
	{
		JSValue func = obj->Get(exec, propertyName);
		JSCallInfo callInfo = func.GetCallInfo(exec);

		if(callInfo.type == kJSCallTypeNone)
		{
			if(exec->GetGlobalObject()->HasException())
				return exec->GetGlobalObject()->GetException();

			jsThrowNativeErrorWithMessage(kJSTypeError, "Function does not have any call info.");
			return jsUndef();
		}

		if(exec->GetGlobalObject()->HasException())
			return exec->GetGlobalObject()->GetException();

		JSValue result = exec->ExecuteCall(func.GetAsNativeObject(), callInfo, obj, JSNoArgs);

		MO_ASSERT(!result.IsGetterOrSetter());

		if(exec->GetGlobalObject()->HasException())
			return exec->GetGlobalObject()->GetException();

		if(result.IsObject())
			return JSValue();

		return result;
	}
}