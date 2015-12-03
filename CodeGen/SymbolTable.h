#ifndef MOENJIN_CODEGEN_SYMBOLTABLE_H
#define MOENJIN_CODEGEN_SYMBOLTABLE_H

#include "Platform/Configuration.h"
#include "CodeGen/IRValue.h"
#include <llvm/ADT/StringMap.h>
#include <llvm/IR/Value.h>

namespace MoEnjin
{
	class SymbolTable
	{
		public:
			typedef llvm::StringMap<IRValue> ValueMap;
			typedef ValueMap::iterator iterator;
			typedef ValueMap::const_iterator const_iterator;

		public:
			SymbolTable();

			mo_int32 Size() const { return mValues.size(); }

			mo_bool IsEmpty() const { return mValues.empty(); }
			mo_bool Exists(const UTF8String& name) const;

			iterator Find(const UTF8String& name) { return mValues.find(name.GetChars()); }
			const_iterator Find(const UTF8String& name) const { return mValues.find(name.GetChars()); }

			IRValue Get(const UTF8String& name) const;
			IRValue Get(const UTF8String& name);
			IRValue Set(const UTF8String& name, const IRValue& value);

			void Remove(const UTF8String& name);
			void Clear();

			iterator Begin() { return mValues.begin(); }
			const_iterator Begin() const { return mValues.begin(); }

			iterator End() { return mValues.end(); }
			const_iterator End() const { return mValues.end(); }

		private:
			ValueMap mValues;
	};
}

#endif