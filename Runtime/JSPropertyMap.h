#ifndef MOENJIN_RUNTIME_JSPROPERTYMAP_H
#define MOENJIN_RUNTIME_JSPROPERTYMAP_H

#include "Platform/Configuration.h"
#include "Runtime/JSValueWrapper.h"

#include <llvm/ADT/StringMap.h>

namespace MoEnjin
{
	struct JSPropertyMapEntry
	{
		UTF8String* name;
		mo_unsigned attributes;
		JSValueWrapper<JSAnyValue> value;

		JSPropertyMapEntry()
			: name(0)
			, attributes(0) { }

		JSPropertyMapEntry(UTF8String* name, JSValue value, mo_unsigned attributes)
			: name(name)
			, attributes(attributes)
			, value(value) { }
	};

	class JSPropertyMap
	{
		typedef llvm::StringMap<JSPropertyMapEntry> PropertyStringMap;

		public:
			typedef PropertyStringMap::iterator iterator;
			typedef PropertyStringMap::const_iterator const_iterator;

			const PropertyStringMap& GetTable() const { return mTable; }

		public:
			JSPropertyMap();
			JSPropertyMap(mo_unsigned capacity);

			void Clear();
			mo_int32 GetCount() const;
			mo_bool IsEmpty() const;

			const JSPropertyMapEntry* Get(const UTF8String& name) const;
			const JSValueWrapper<JSAnyValue>* GetDirectValue(const UTF8String& name) const;
			JSValue GetValue(const UTF8String& name) const;
			

			mo_bool Set(const UTF8String& name, JSValue value, mo_unsigned attributes);

			mo_bool Remove(const UTF8String& name);
			mo_bool Contains(const UTF8String& name);

		private:
			PropertyStringMap mTable;
	};
}

#endif