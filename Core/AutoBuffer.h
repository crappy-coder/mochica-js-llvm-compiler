#ifndef MOENJIN_CORE_AUTOBUFFER_H
#define MOENJIN_CORE_AUTOBUFFER_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	MO_TEMPLATE_WITH_TYPE(T)
	class AutoBuffer
	{
		public:
			explicit AutoBuffer()
				: mPtr(mo_null) { }

			explicit AutoBuffer(T* ptr)
				: mPtr(ptr) { }

			~AutoBuffer()
			{
				if(mPtr)
					delete[] mPtr;
			}

			T& operator [](int index) { return mPtr[index]; }
			T* operator *() { return mPtr; }
			T* operator =(T* value)
			{
				if(mPtr)
					delete[] mPtr;

				mPtr = value;
				return mPtr;
			}

		private:
			T* mPtr;
	};
}

#endif