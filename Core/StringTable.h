#ifndef MOENJIN_CORE_STRINGTABLE_H
#define MOENJIN_CORE_STRINGTABLE_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	struct StringTableValue
	{
		StringTableValue()
			: value(0), id(0) { }

		StringTableValue(const StringTableValue& other)
			: value(other.value), id(other.id) { }

		StringTableValue(const UTF8String& value,  mo_atom id = 0)
			: value(value), id(id) { }

		mo_inline mo_bool operator == (const StringTableValue& b)
		{
			// first try by identifier
			if(this->id != 0 && b.id != 0)
				return (this->id == b.id);

			// then exclude non equal lengths
			if(this->value.GetLength() != b.value.GetLength())
				return false;

			// one or both is null, allow two nulls to
			// succeed
			if(this->value.IsNull() || b.value.IsNull())
				return (this->value.IsNull() == b.value.IsNull());

			return (this->value == b.value);
		}

		mo_inline mo_bool operator != (const StringTableValue& b)
		{
			return !((*this) == b);
		}

		friend mo_bool operator == (const StringTableValue& a, const StringTableValue& b);
		friend mo_bool operator != (const StringTableValue& a, const StringTableValue& b);

		mo_atom id;
		UTF8String value;
	};

	class StringTable
	{
		static const mo_int32 MIN_LOAD = 4;
		static const mo_int32 MAX_LOAD = 2;
		static const mo_int32 MIN_TABLE_SIZE = 64;
		static const mo_int32 BAD_INDEX = -1;

		public:
			static const UTF8String NOT_FOUND;

		public:
			StringTable(mo_uint32 initialCapacity = MIN_TABLE_SIZE);
			StringTable(mo_utf8stringptr* strings, mo_uint32 stringsLength);

			mo_int32 GetSize() const { return mKeyCount; }
			mo_int32 GetCapacity() const { return mTableSize; }

			mo_bool IsEmpty() const { return (mKeyCount == 0); }

			mo_bool Exists(const UTF8String& value);
			mo_bool Exists(const mo_utf8stringptr value, const mo_int32 len);

			const UTF8String& Get(const mo_atom id) const;
			const UTF8String& GetAt(const mo_uint32 index) const;

			mo_atom Add(const UTF8String& value);
			mo_atom Add(const mo_utf8stringptr value, const mo_int32 len);

			mo_bool Remove(const mo_atom id);
			void Clear();

		private:
			void EnsureTable();
			void Rehash(mo_uint32 newSize);
			void Grow();
			void Shrink();
			
			mo_bool CanGrow() const;
			mo_bool CanShrink() const;
			mo_bool HasValue(mo_uint32 index) const;

			mo_int32 GetIndex(mo_utf8stringptr str, mo_uint32 len, mo_uint32* hash) const;
			mo_int32 GetIndexForHash(mo_uint32 hash) const;

			StringTableValue* AllocateTable(mo_uint32 size);

			StringTableValue* mTable;
			mo_uint32 mTableSize;
			mo_uint32 mInitialCapacity;
			mo_uint32 mKeyCount;
	};

	mo_inline mo_bool operator == (const StringTableValue& a, const StringTableValue& b)
	{
		return (a == b);
	}

	mo_inline mo_bool operator != (const StringTableValue& a, const StringTableValue& b)
	{
		return (a != b);
	}
}

#endif