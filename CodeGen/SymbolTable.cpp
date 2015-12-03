#include "CodeGen/SymbolTable.h"

namespace MoEnjin
{
	SymbolTable::SymbolTable() { }

	mo_bool SymbolTable::Exists(const UTF8String& name) const
	{
		return (!mValues.empty() && mValues.find(name.GetChars()) != mValues.end());
	}

	IRValue SymbolTable::Get(const UTF8String& name) const
	{
		const_iterator result = mValues.find(name.GetChars());

		if(result != mValues.end())
			return result->second;

		return IRValue::Null;
	}

	IRValue SymbolTable::Get(const UTF8String& name)
	{
		iterator result = mValues.find(name.GetChars());

		if(result != mValues.end())
			return result->second;

		return IRValue::Null;
	}

	IRValue SymbolTable::Set(const UTF8String& name, const IRValue& value)
	{
		iterator result = mValues.find(name.GetChars());

		if(result == mValues.end())
		{
			mValues.insert(ValueMap::MapEntryTy::Create<IRValue>(name.GetChars(), name.GetChars() + name.GetLength(), value));
			return Get(name);
		}

		result->second.Set(value.Get());
		return result->second;
	}

	void SymbolTable::Remove(const UTF8String& name)
	{
		mValues.erase(name.GetChars());
	}

	void SymbolTable::Clear()
	{
		mValues.clear();
	}
}