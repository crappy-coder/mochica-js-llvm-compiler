#include "CodeGen/IRFunctionTable.h"

namespace MoEnjin
{
	IRFunctionTable::IRFunctionTable()
	{

	}

	mo_bool IRFunctionTable::Exists(const UTF8String& name) const
	{
		return (!mValues.empty() && mValues.find(name.GetChars()) != mValues.end());
	}

	const IRFunctionTableEntry* IRFunctionTable::GetEntry(const UTF8String& name, mo_int32 id)
	{
		const_iterator result = mValues.find(name.GetChars());
		
		if(result != mValues.end())
		{
			for(const_entry_iterator i = result->second.begin(); i != result->second.end(); ++i)
			{
				if(i->id == id)
					return &(*i);
			}
		}

		return mo_null;
	}

	IRFunction IRFunctionTable::GetLatest(const UTF8String& name)
	{
		IRFunction function = IRFunction::Null;
		const_iterator result = mValues.find(name.GetChars());

		if(result != mValues.end())
		{
			const IRFunctionTableEntry* entry = mo_null;

			for(const_entry_iterator i = result->second.begin(); i != result->second.end(); ++i)
			{
				if(!entry || entry->id < i->id)
					entry = &(*i);
			}

			if(entry)
				function = entry->function;
		}

		return function;
	}

	IRFunction IRFunctionTable::Get(const UTF8String& name, mo_int32 id)
	{
		const IRFunctionTableEntry* entry = GetEntry(name, id);

		return entry ? entry->function : IRFunction::Null;
	}

	void IRFunctionTable::Add(const UTF8String& name, const IRFunction& function, mo_int32 sourceLocation)
	{
		mo_int32 nextId = 0;
		IRFunctionTableEntryCollection* entries = &mValues.GetOrCreateValue(name.GetChars()).second;

		if(!entries->empty())
		{
			IRFunction latestFunc;

			for(const_entry_iterator i = entries->begin(); i != entries->end(); ++i)
			{
				if(i->id > nextId)
				{
					nextId = i->id;
					latestFunc = i->function;
				}	
			}

			if(!latestFunc.IsNull())
			{
				UTF8String newName = name;
				newName += "_";
				newName += nextId;

				latestFunc->setName(newName.GetChars());
			}
		}

		entries->push_back(IRFunctionTableEntry(name, nextId+1, sourceLocation, function));
	}

	void IRFunctionTable::Remove(const UTF8String& name)
	{
		mValues.erase(name.GetChars());
	}

	void IRFunctionTable::Clear()
	{
		mValues.clear();
	}
}