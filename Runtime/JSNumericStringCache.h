#ifndef MOENJIN_RUNTIME_JSNUMERICSTRINGCACHE_H
#define MOENJIN_RUNTIME_JSNUMERICSTRINGCACHE_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	// TODO - update JSNumericStringCache to a static or singelton class
	//

	class JSNumericStringCache
	{
		static const mo_uint32 CACHE_SIZE = 64;

		template <typename T>
		struct CacheEntry
		{
			T key;
			UTF8String value;
		};

		public:
			mo_inline_always UTF8String Add(mo_double v)
			{
				CacheEntry<mo_double>& e = Find(v);

				if(v != e.key || e.value.IsNull())
				{
					e.key = v;
					e.value = UTF8String::FromNumber(v);
				}

				return e.value;
			}

			mo_inline_always UTF8String Add(mo_int32 v)
			{
				if(mo_cast<mo_unsigned>(v) < CACHE_SIZE)
					return FindTinyInt(mo_cast<mo_unsigned>(v));

				CacheEntry<mo_int32>& e = Find(v);

				if(v != e.key || e.value.IsNull())
				{
					e.key = v;
					e.value = UTF8String::FromNumber(v);
				}

				return e.value;
			}

			mo_inline_always UTF8String Add(mo_unsigned v)
			{
				if(v < CACHE_SIZE)
					return FindTinyInt(v);

				CacheEntry<mo_unsigned>& e = Find(v);

				if(v != e.key || e.value.IsNull())
				{
					e.key = v;
					e.value = UTF8String::FromNumber(v);
				}

				return e.value;
			}

		private:
			CacheEntry<mo_double>& Find(mo_double v) 
			{ 
				return mDoubleCache[Hash::HashFloat(v) & (CACHE_SIZE - 1)];
			}

			CacheEntry<mo_int32>& Find(mo_int32 v) 
			{ 
				return mIntCache[Hash::HashInt(v) & (CACHE_SIZE - 1)];
			}

			CacheEntry<mo_unsigned>& Find(mo_unsigned v) 
			{ 
				return mUIntCache[Hash::HashInt(v) & (CACHE_SIZE - 1)];
			}

			mo_inline_always const UTF8String& FindTinyInt(mo_unsigned v)
			{
				MO_ASSERT(v < CACHE_SIZE);

				if(mTinyIntCache[v].IsEmpty())
					mTinyIntCache[v] = UTF8String::FromNumber(v);

				return mTinyIntCache[v];
			}

			CacheEntry<mo_double> mDoubleCache[CACHE_SIZE];
			CacheEntry<mo_int32> mIntCache[CACHE_SIZE];
			CacheEntry<mo_unsigned> mUIntCache[CACHE_SIZE];
			UTF8String mTinyIntCache[CACHE_SIZE];
	};
}

#endif