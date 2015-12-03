#include "Runtime/JSPropertyMap.h"

namespace MoEnjin
{
	JSPropertyMap::JSPropertyMap() { }

	JSPropertyMap::JSPropertyMap(mo_unsigned capacity) { }

	mo_int32 JSPropertyMap::GetCount() const
	{
		return mTable.size();
	}

	const JSPropertyMapEntry* JSPropertyMap::Get(const UTF8String& name) const
	{
		const JSPropertyMapEntry* entry = mo_null;
		PropertyStringMap::const_iterator result = mTable.find(name.GetChars());

		if(result != mTable.end())
			entry = &result->second;

		return entry;
	}

	JSValue JSPropertyMap::GetValue(const UTF8String& name) const
	{
		const JSPropertyMapEntry* entry = Get(name);

		if(!entry)
			return JSValue();

		return entry->value.Get();
	}

	const JSValueWrapper<JSAnyValue>* JSPropertyMap::GetDirectValue(const UTF8String& name) const
	{
		const JSPropertyMapEntry* entry = Get(name);

		if(!entry)
			return mo_null;

		return &entry->value;
	}

	mo_bool JSPropertyMap::Set(const UTF8String& name, JSValue value, mo_unsigned attributes)
	{
		PropertyStringMap::MapEntryTy* entry = mo_null;
		PropertyStringMap::iterator result = mTable.find(name.GetChars());
		mo_bool success = mo_false;

		if(result == mTable.end())
		{
			entry = PropertyStringMap::MapEntryTy::Create<JSPropertyMapEntry>(name.GetChars(), name.GetChars() + name.GetLength(), JSPropertyMapEntry(new UTF8String(name), value, attributes));
			success = mTable.insert(entry);
		}
		else
		{
			result->second.attributes = attributes;
			result->second.value.Set(value);

			success = mo_true;
		}

		return success;
	}

	mo_bool JSPropertyMap::Remove(const UTF8String& name)
	{
		return mTable.erase(name.GetChars());	
	}

	void JSPropertyMap::Clear()
	{
		mTable.clear();
	}

	mo_bool JSPropertyMap::IsEmpty() const
	{
		return mTable.empty();
	}

	mo_bool JSPropertyMap::Contains(const UTF8String& name)
	{
		return (!mTable.empty() && mTable.find(name.GetChars()) != mTable.end());
	}
}