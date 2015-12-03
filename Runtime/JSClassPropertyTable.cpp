#include "Runtime/JSClassPropertyTable.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSFunction.h"

namespace MoEnjin
{
	void JSClassPropertyTable::Initialize() const
	{
		if(mIsInitialized)
			return;

		for(mo_int32 i = 0; mValues[i].propertyName; ++i)
		{
			UTF8String propertyName(mValues[i].propertyName);
			PropertyStringMap::MapEntryTy* entry = PropertyStringMap::MapEntryTy::Create<JSClassPropertyTableEntry>(propertyName.GetChars(), propertyName.GetChars() + propertyName.GetLength(), JSClassPropertyTableEntry(propertyName, mValues[i].attributes, mValues[i].value1, mValues[i].value2));

			mTable.insert(entry);
		}

		mIsInitialized = mo_true;
	}

	void JSClassPropertyTable::EnsureInitialized() const
	{
		Initialize();
	}

	const JSClassPropertyTableEntry* JSClassPropertyTable::GetEntry(const UTF8String& name) const
	{
		Initialize();

		const JSClassPropertyTableEntry* entry = mo_null;
		PropertyStringMap::const_iterator result = mTable.find(name.GetChars());

		if(result != mTable.end())
			entry = &result->second;

		return entry;
	}

	mo_bool InitializeStaticFunctionSlot(JSStackFrame* exec, const JSClassPropertyTableEntry* entry, JSObject* thisObj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		MO_ASSERT(thisObj->GetGlobalObject());
		MO_ASSERT(entry->GetAttributes() & JSPropertyAttributeFunction);

		JSValue value = thisObj->GetValue(propertyName);
		
		if(!value)
		{
			JSFunction* func = JSFunction::Create(exec, propertyName, entry->GetFunctionLength(), entry->GetFunction());
			thisObj->Put(exec, propertyName, func, entry->GetAttributes());
			value = thisObj->GetValue(propertyName);
		}

		slot.SetValue(thisObj, value);
		return mo_true;
	}
}