#ifndef MOENJIN_RUNTIME_JSPUTPROPERTYSLOT_H
#define MOENJIN_RUNTIME_JSPUTPROPERTYSLOT_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class JSObject;
	class JSPutPropertySlot
	{
		public:
			JSPutPropertySlot(mo_bool isStrictMode = mo_false)
				: mIsExisting(mo_false)
				, mBase(mo_null)
				, mIsStrictMode(isStrictMode) { }

			mo_bool IsExisting() const { return mIsExisting; }
			mo_bool IsStrictMode() const { return mIsStrictMode; }

			JSObject* GetBase() const { return mBase; }

			void SetExisting(JSObject* base)
			{
				mIsExisting = mo_true;
				mBase = base;
			}

			void SetNew(JSObject* base)
			{
				mIsExisting = mo_false;
				mBase = base;
			}

		private:
			mo_bool mIsExisting;
			mo_bool mIsStrictMode;
			JSObject* mBase;		
	};
}

#endif