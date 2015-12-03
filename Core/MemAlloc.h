#ifndef MOENJIN_CORE_MEMALLOC_H
#define MOENJIN_CORE_MEMALLOC_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class MemAlloc
	{
		MO_STATIC_CLASS(MemAlloc);

		public:
			template <typename T>
			static mo_bool TryMalloc(size_t len, T& outData)
			{
				void* data = ::malloc(len);

				union u { 
					void* p;
					T o;
				} r;

				r.p = data;
				outData = r.o;

				return !!data;
			}
	};
}

#endif