#ifndef MOENJIN_CODEGEN_IRFUNCTIONTABLE_H
#define MOENJIN_CODEGEN_IRFUNCTIONTABLE_H

#include "Platform/Configuration.h"
#include "CodeGen/IRValue.h"
#include <llvm/ADT/StringMap.h>

namespace MoEnjin
{
	struct IRFunctionTableEntry
	{
		UTF8String name;
		mo_int32 id;
		mo_int32 sourceLocation;
		IRFunction function;

		IRFunctionTableEntry()
			: id(0), sourceLocation(-1) { }

		IRFunctionTableEntry(const UTF8String& name, mo_int32 id, mo_int32 sourceLocation, const IRFunction& function)
			: name(name)
			, id(id)
			, sourceLocation(sourceLocation)
			, function(function) { }
	};

	class IRFunctionTable
	{
		typedef std::vector<IRFunctionTableEntry> IRFunctionTableEntryCollection;

		public:
			typedef llvm::StringMap<IRFunctionTableEntryCollection> ValueMap;
			typedef ValueMap::iterator iterator;
			typedef ValueMap::const_iterator const_iterator;
			typedef IRFunctionTableEntryCollection::iterator entry_iterator;
			typedef IRFunctionTableEntryCollection::const_iterator const_entry_iterator;

		public:
			IRFunctionTable();

			mo_int32 Size() const { return mValues.size(); }
			mo_int32 Size(const UTF8String& name) { return mValues.count(name.GetChars()); }

			mo_bool IsEmpty() const { return mValues.empty(); }
			mo_bool Exists(const UTF8String& name) const;

			const IRFunctionTableEntry* GetEntry(const UTF8String& name, mo_int32 id);
			IRFunction GetLatest(const UTF8String& name);
			IRFunction Get(const UTF8String& name, mo_int32 id);

			void Add(const UTF8String& name, const IRFunction& function, mo_int32 sourceLocation);

			void Remove(const UTF8String& name);
			void Clear();

		private:
			ValueMap mValues;
	};
}

#endif