#ifndef MOENJIN_CORE_IID_H
#define MOENJIN_CORE_IID_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class IID
	{
		public:
			IID() : mCurrentId(0) { }
			IID(mo_uint32 startId) : mCurrentId(startId) { }

			const mo_uint32 GetCurrentId() const { return mCurrentId; }
			const mo_uint32 GetNextId() { return mCurrentId++; }

		private:
			mo_uint32 mCurrentId;
	};
}

#endif