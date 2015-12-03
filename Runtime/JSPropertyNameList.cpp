#include "Runtime/JSPropertyNameList.h"

namespace MoEnjin
{
	void JSPropertyNameList::Add(const UTF8String& name, mo_bool direct)
	{
		// check for duplicates, otherwise just add it 
		// if the caller knows there won't be any duplicates
		if(!direct)
		{
			mo_int32 len = mNames.size();

			// ECMA 5.1, §12.6.4
			// cannot have duplicate names
			for(mo_int32 i = 0; i < len; ++i)
			{
				if(name == mNames[i])
					return;
			}
		}

		mNames.push_back(name);
	}
}