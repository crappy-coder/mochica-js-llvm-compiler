#ifndef MOENJIN_RUNTIME_JSLIST_H
#define MOENJIN_RUNTIME_JSLIST_H

#include "Platform/Configuration.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSList
	{
		typedef std::vector<JSValue> ValueCollection;

		public:
			JSList() { }

			JSList(const JSList& other)
				: mValues(other.mValues) { }

			JSList(JSValue arg)
				: mValues(1, arg) { }

			JSList(JSValue* values, mo_uint32 length)
			{
				for(mo_int32 i = 0; i < length; ++i)
					mValues.push_back(values[i]);
			}

			mo_bool IsEmpty() const { return mValues.empty(); }
			mo_uint32 GetLength() const { return mValues.size(); }
			const JSValue* GetValues() const { return mValues.data(); }

			void Add(JSValue value) { mValues.push_back(value); }
			void Clear() { mValues.clear(); }

			JSValue Get(mo_uint32 index) const
			{
				if(index >= mValues.size())
					return jsUndef();

				return mValues.at(index);
			}

		private:
			ValueCollection mValues;
	};

	typedef JSList JSArgList;
}

#endif