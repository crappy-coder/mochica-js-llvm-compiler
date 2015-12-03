#ifndef MOENJIN_RUNTIME_JSPROPERTYNAMELIST_H
#define MOENJIN_RUNTIME_JSPROPERTYNAMELIST_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class JSPropertyNameList
	{
		public:
			JSPropertyNameList() { }

			mo_uint32 GetCount() const { return mNames.size(); }
			void Add(const UTF8String& name, mo_bool direct = mo_false);

			UTF8String& operator[](mo_uint32 index) { return mNames[index]; }
			const UTF8String& operator[](mo_uint32 index) const { return mNames[index]; }

		private:
			UTF8StringCollection mNames;
	};
}

#endif