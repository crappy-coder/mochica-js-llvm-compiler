#include "Core/StringTable.h"
#include "Core/Hash.h"

namespace MoEnjin
{
	const UTF8String StringTable::NOT_FOUND = UTF8String::MakeNull();

	StringTable::StringTable(mo_uint32 initialCapacity)
		: mInitialCapacity(initialCapacity)
		, mTable(mo_null)
		, mTableSize(0)
		, mKeyCount(0) { }

	StringTable::StringTable(mo_utf8stringptr* strings, mo_uint32 stringsLength)
		: mInitialCapacity(MIN_TABLE_SIZE)
		, mTable(mo_null)
		, mTableSize(stringsLength)
		, mKeyCount(0)
	{ 
		Grow();

		for(mo_int32 i = 0; i < stringsLength; ++i)
		{
			mo_utf8stringptr str = strings[i];
			mo_uint32 len = strlen(str);
			mo_uint32 hash = 0;
			mo_int32 idx = GetIndex(str, len, &hash);
			StringTableValue* value = mo_null;

			if(idx == BAD_INDEX)
				continue;

			mTable[idx] = StringTableValue(UTF8String(str), hash);
		}
	}

	mo_bool StringTable::Exists(const UTF8String& value)
	{
		if(!mTable || value.IsEmpty())
			return mo_false;

		mo_uint32 hash = value.GetHash();
		mo_int32 idx = GetIndexForHash(hash);

		if(idx == BAD_INDEX || !HasValue(idx))
			return mo_false;

		return mo_true;
	}

	mo_bool StringTable::Exists(const mo_utf8stringptr value, const mo_int32 len)
	{
		return Exists(UTF8String(value, len));
	}

	const UTF8String& StringTable::Get(const mo_atom id) const
	{
		if(!mTable)
			return NOT_FOUND;

		mo_int32 idx = GetIndexForHash(id);

		if(idx == BAD_INDEX || !HasValue(idx))
			return NOT_FOUND;

		StringTableValue* v = &mTable[idx];

		return v->value;
	}

	const UTF8String& StringTable::GetAt(const mo_uint32 index) const
	{
		if(index < mKeyCount)
		{
			mo_uint32 n = 0;

			for(int i = 0; i < mTableSize; ++i)
			{
				if(!HasValue(i))
					continue;

				if(n == index)
					return mTable[i].value;

				n++;
			}
		}

		return NOT_FOUND;
	}

	mo_atom StringTable::Add(const UTF8String& value)
	{
		EnsureTable();

		if(value.IsEmpty())
			return mo_bad_atom;

		mo_uint32 hash = value.GetHash();
		mo_int32 idx = GetIndexForHash(hash);

		if(idx == BAD_INDEX)
			return mo_bad_atom;

		if(!HasValue(idx))
		{
			mTable[idx] = StringTableValue(value, hash);
			mKeyCount++;

			Grow();
		}

		return hash;
	}

	mo_atom StringTable::Add(const mo_utf8stringptr value, const mo_int32 len)
	{
		return Add(UTF8String(value, len));
	}

	mo_bool StringTable::Remove(const mo_atom id)
	{
		if(!mTable)
			return mo_false;

		mo_int32 idx = GetIndexForHash(id);

		if(idx == BAD_INDEX || !HasValue(idx))
			return mo_false;

		mTable[idx].id = 0;
		mTable[idx].value = UTF8String(0);
		mKeyCount--;

		Shrink();

		return mo_true;
	}

	void StringTable::Clear()
	{
		if(!mTable)
			return;

		// TODO : clean up strings?
		free(mTable);

		mTable = mo_null;
		mTableSize = 0;
		mKeyCount = 0;
	}

	void StringTable::EnsureTable()
	{
		Grow();
	}

	void StringTable::Rehash(mo_uint32 newSize)
	{
		mo_uint32 currentTableSize = mTableSize;
		StringTableValue* currentTable = mTable;

		mTableSize = newSize;
		mTable = AllocateTable(newSize);

		for(int i = 0; i != currentTableSize; ++i)
		{
			if(currentTable[i].value.IsNull())
				continue;

			mo_int32 idx = GetIndexForHash(currentTable[i].id);

			if(idx == BAD_INDEX)
				continue;

			mTable[idx] = StringTableValue(currentTable[i]);
		}

		if(currentTable)
			free(currentTable);
	}

	void StringTable::Grow()
	{
		if(mTable && !CanGrow())
			return;

		mo_uint32 newSize = 0;

		if(mTableSize == 0)
			newSize = mInitialCapacity;
		else if((mKeyCount * MIN_LOAD) < (mTableSize * 2))
			newSize = mTableSize;
		else
			newSize = mTableSize * 2;

		Rehash(newSize);
	}

	void StringTable::Shrink()
	{
		if(!CanShrink())
			return;

		Rehash(mTableSize / 2);
	}

	mo_bool StringTable::CanGrow() const
	{
		return ((mKeyCount * MAX_LOAD) >= mTableSize);
	}

	mo_bool StringTable::CanShrink() const
	{
		return ((mKeyCount * MIN_LOAD) < mTableSize && mTableSize > MIN_TABLE_SIZE);
	}

	mo_bool StringTable::HasValue(mo_uint32 index) const
	{
		return !mTable[index].value.IsNull();
	}

	mo_int32 StringTable::GetIndex(mo_utf8stringptr str, mo_uint32 len, mo_uint32* hash) const
	{
		if(!str)
		{
			*hash = 0;
			return -1;
		}

		*hash = Hash::HashString(str, len);
		return GetIndexForHash(*hash);
	}

	mo_int32 StringTable::GetIndexForHash(mo_uint32 hash) const
	{
		if(hash == 0)
			return -1;

		return (hash % mTableSize);
	}

	StringTableValue* StringTable::AllocateTable(mo_uint32 size)
	{
		return new StringTableValue[size];
		//mo_uint32 dataSize = (size * sizeof(StringTableValue));
		//StringTableValue* table = static_cast<StringTableValue*>(malloc(dataSize));
		//
		//memset(table, 0, dataSize);

		//return table;
	}
}