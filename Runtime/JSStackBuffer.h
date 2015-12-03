#ifndef MOENJIN_RUNTIME_JSSTACKBUFFER_H
#define MOENJIN_RUNTIME_JSSTACKBUFFER_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class JSCallFrame;
	class JSStackBuffer
	{
		MO_NON_COPYABLE_CLASS(JSStackBuffer);
			
		public:
			static const mo_uint32 DefaultCapacity = 1024 * 512; // 512k
			
		public:
			JSStackBuffer(mo_uint32 capacity = DefaultCapacity);

			JSCallFrame* GetBegin() const { return mBegin; }
			JSCallFrame* GetEnd() const { return mEnd; }
			JSCallFrame* GetPosition() const { return mPosition; }
			mo_uint32 GetCapacity() const { return mCapacity; }
			mo_uint32 GetSize() const;

			mo_bool CanGrow(mo_uint32 size = 0) const;

			mo_bool Grow(mo_uint32 size);
			mo_bool Update(JSCallFrame* position);

			void Shrink(mo_uint32 size);

		private:
			JSCallFrame* mBegin;
			JSCallFrame* mEnd;
			JSCallFrame* mPosition;
			mo_uint32 mCapacity;
	};
}

#endif