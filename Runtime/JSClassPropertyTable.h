#ifndef MOENJIN_RUNTIME_JSCLASSPROPERTYTABLE_H
#define MOENJIN_RUNTIME_JSCLASSPROPERTYTABLE_H

#include "Platform/Configuration.h"
#include "Runtime/JSPropertyAttributes.h"
#include "Runtime/JSPropertyDescriptor.h"
#include "Runtime/JSPropertySlot.h"
#include "Runtime/JSValue.h"

#include <llvm/ADT/StringMap.h>

namespace MoEnjin
{
	class JSObject;
	class JSStackFrame;

	struct JSClassPropertyTableValue
	{
		const mo_utf8stringptr propertyName;
		mo_byte attributes;
		mo_intptr value1;
		mo_intptr value2;
	};

	typedef GetValueFunction PropertyGetFunction;
	typedef void (*PropertySetFunction)(JSStackFrame* stackFrame, JSObject* baseObject, JSValue value);

	#define NATIVE_GET_FUNCTION_PTR(f,c)	((mo_intptr)mo_cast<PropertyGetFunction>(f)), ((mo_intptr)c)
	#define NATIVE_SET_FUNCTION_PTR(f,c)	((mo_intptr)mo_cast<PropertySetFunction>(f)), ((mo_intptr)c)

	struct JSClassPropertyTableEntry
	{
		public:
			JSClassPropertyTableEntry()
				: mPropertyName()
				, mAttributes(0)
			{
				mData.values.value1 = 0;
				mData.values.value2 = 0;
			}

			JSClassPropertyTableEntry(const UTF8String& propertyName, mo_byte attributes, mo_intptr v1, mo_intptr v2)
			{
				mPropertyName = propertyName;
				mAttributes = attributes;
				mData.values.value1 = v1;
				mData.values.value2 = v2;
			}

			void SetPropertyName(const UTF8String& name) { mPropertyName = name; }
			const UTF8String& GetPropertyName() const { return mPropertyName; }

			mo_byte GetAttributes() const { return mAttributes; }

			NativeFunctionPtr GetFunction() const { MO_ASSERT(mAttributes & JSPropertyAttributeFunction); return mData.function.function; }
			mo_byte GetFunctionLength() const { MO_ASSERT(mAttributes & JSPropertyAttributeFunction); return mo_cast<mo_byte>(mData.function.length); }

			PropertyGetFunction GetPropertyGetter() const { MO_ASSERT(!(mAttributes & JSPropertyAttributeFunction)); return mData.property.get; }
			PropertySetFunction GetPropertySetter() const { MO_ASSERT(!(mAttributes & JSPropertyAttributeFunction)); return mData.property.set; }

		private:
			UTF8String mPropertyName;
			mo_byte mAttributes;

			union {
				struct {
					mo_intptr value1;
					mo_intptr value2;
				} values;
				struct {
					NativeFunctionPtr function;
					mo_intptr length;
				} function;
				struct {
					PropertyGetFunction get;
					PropertySetFunction set;
				} property;
			} mData;
	};

	struct JSClassPropertyTable
	{
		typedef llvm::StringMap<JSClassPropertyTableEntry> PropertyStringMap;

		// data section
		const JSClassPropertyTableValue* mValues;
		mutable mo_bool mIsInitialized;
		mutable PropertyStringMap mTable;

		public:
			typedef PropertyStringMap::iterator iterator;
			typedef PropertyStringMap::const_iterator const_iterator;

			const JSClassPropertyTableEntry* GetEntry(const UTF8String& name) const;
			void EnsureInitialized() const;
						
		private:
			void Initialize() const;
	};

	mo_bool InitializeStaticFunctionSlot(JSStackFrame* exec, const JSClassPropertyTableEntry* entry, JSObject* thisObj, const UTF8String& propertyName, JSPropertySlot& slot);

	template<class ThisImpl, class ParentImpl>
	mo_inline mo_bool GetStaticPropertySlot(JSStackFrame* exec, const JSClassPropertyTable* table, ThisImpl* thisObj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		const JSClassPropertyTableEntry* entry = table->GetEntry(propertyName);

		if(!entry)
			return ParentImpl::GetOwnPropertySlotImpl(exec, thisObj, propertyName, slot);

		if(entry->GetAttributes() & JSPropertyAttributeFunction)
			return InitializeStaticFunctionSlot(exec, entry, thisObj, propertyName, slot);

		slot.SetValueFunc(thisObj, entry->GetPropertyGetter());
		return mo_true;
	}

	template<class ThisImpl, class ParentImpl>
	mo_inline mo_bool GetStaticPropertyDescriptor(JSStackFrame* exec, const JSClassPropertyTable* table, ThisImpl* thisObj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		const JSClassPropertyTableEntry* entry = table->GetEntry(propertyName);

		if(!entry)
			return ParentImpl::GetOwnPropertyDescriptorImpl(exec, thisObj, propertyName, descriptor);

		JSPropertySlot slot;

		if(entry->GetAttributes() & JSPropertyAttributeFunction)
		{
			mo_bool exists = InitializeStaticFunctionSlot(exec, entry, thisObj, propertyName, slot);

			if(exists)
				descriptor.Set(slot.GetValue(exec, propertyName), entry->GetAttributes());

			return exists;
		}

		slot.SetValueFunc(thisObj, entry->GetPropertyGetter());
		descriptor.Set(slot.GetValue(exec, propertyName), entry->GetAttributes());

		return mo_true;
	}

	template<class ParentImpl>
	mo_inline mo_bool GetStaticFunctionSlot(JSStackFrame* exec, const JSClassPropertyTable* table, JSObject* thisObj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		if(ParentImpl::GetOwnPropertySlotImpl(exec, thisObj, propertyName, slot))
			return mo_true;

		const JSClassPropertyTableEntry* entry = table->GetEntry(propertyName);

		if(!entry)
			return mo_false;

		return InitializeStaticFunctionSlot(exec, entry, thisObj, propertyName, slot);
	}

	template<class ParentImpl>
	mo_inline mo_bool GetStaticFunctionDescriptor(JSStackFrame* exec, const JSClassPropertyTable* table, JSObject* thisObj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		if(ParentImpl::GetOwnPropertyDescriptorImpl(exec, thisObj, propertyName, descriptor))
			return mo_true;

		const JSClassPropertyTableEntry* entry = table->GetEntry(propertyName);

		if(!entry)
			return mo_false;

		JSPropertySlot slot;

		mo_bool exists = InitializeStaticFunctionSlot(exec, entry, thisObj, propertyName, slot);

		if(exists)
			descriptor.Set(slot.GetValue(exec, propertyName), entry->GetAttributes());

		return exists;
	}

	template<class ThisImpl, class ParentImpl>
	mo_inline mo_bool GetStaticValueSlot(JSStackFrame* exec, const JSClassPropertyTable* table, ThisImpl* thisObj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		const JSClassPropertyTableEntry* entry = table->GetEntry(propertyName);

		if(!entry)
			return ParentImpl::GetOwnPropertySlotImpl(exec, thisObj, propertyName, slot);

		MO_ASSERT(!(entry->GetAttributes() & JSPropertyAttributeFunction));

		slot.SetValueFunc(thisObj, entry->GetPropertyGetter());

		return mo_true;
	}

	template<class ThisImpl, class ParentImpl>
	mo_inline mo_bool GetStaticValueSlot(JSStackFrame* exec, const JSClassPropertyTable* table, ThisImpl* thisObj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		const JSClassPropertyTableEntry* entry = table->GetEntry(propertyName);

		if(!entry)
			return ParentImpl::GetOwnPropertyDescriptorImpl(exec, thisObj, propertyName, descriptor);

		MO_ASSERT(!(entry->GetAttributes() & JSPropertyAttributeFunction));

		JSPropertySlot slot;
		slot.SetValueFunc(thisObj, entry->GetPropertyGetter());
		descriptor.Set(slot.GetValue(exec, propertyName), entry->GetAttributes());

		return mo_true;
	}

	template<class ThisImpl>
	mo_inline mo_bool PutStaticValue(JSStackFrame* exec, const JSClassPropertyTable* table, ThisImpl* thisObj, const UTF8String& propertyName, JSValue value, mo_bool throwExceptions = mo_false)
	{
		const JSClassPropertyTableEntry* entry = table->GetEntry(propertyName);

		if(!entry)
			return mo_false;

		if((entry->GetAttributes() & JSPropertyAttributeFunction) == JSPropertyAttributeFunction)
			thisObj->PutProperty(propertyName, value);
		else if((entry->GetAttributes() & JSPropertyAttributeReadOnly) == 0)
			entry->GetPropertySetter()(exec, thisObj, value);
		else if(throwExceptions)
		{
			jsThrowNativeErrorWithMessage(kJSTypeError, "Trying to set a static property value on a read-only property.");
			return mo_false;
		}

		return mo_true;
	}

	template<class ThisImpl, class ParentImpl>
	mo_inline void PutStaticValue(JSStackFrame* exec, const JSClassPropertyTable* table, ThisImpl* thisObj, const UTF8String& propertyName, JSValue value, JSPutPropertySlot& slot)
	{
		if(!PutStaticValue<ThisImpl>(exec, table, thisObj, propertyName, value, slot.IsStrictMode()))
			ParentImpl::PutPropertyImpl(exec, thisObj, propertyName, value, slot.IsStrictMode(), slot);
	}
}

#endif