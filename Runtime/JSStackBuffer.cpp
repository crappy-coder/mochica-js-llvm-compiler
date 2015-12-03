#include "Runtime/JSStackBuffer.h"
#include "Runtime/JSCallFrame.h"

namespace MoEnjin
{
	JSStackBuffer::JSStackBuffer(mo_uint32 capacity)
		: mBegin(mo_null), mEnd(mo_null), mPosition(mo_null), mCapacity(capacity)
	{
		// allocate <size> bytes of memory and keep a reference
		// to the base address
		mBegin = mo_cast<JSCallFrame*>(malloc(capacity));

		// save end address for quicker calculations
		mEnd = mo_cast_type<JSCallFrame*>(mo_cast_type<char*>(mBegin) + mCapacity);

		// start at the front
		mPosition = mBegin;

		// initialize data to zero
		memset(mo_cast_type<char*>(mBegin), 0, mCapacity);
	}

	mo_bool JSStackBuffer::CanGrow(mo_uint32 size) const
	{
		return ((mPosition + size) <= mEnd);
	}

	mo_uint32 JSStackBuffer::GetSize() const
	{
		return mPosition - mBegin;
	}

	mo_bool JSStackBuffer::Grow(mo_uint32 size)
	{
		JSCallFrame* nextPos = mPosition + size;

		if(nextPos > mEnd)
			return mo_false;

		mPosition = nextPos;
		return mo_true;
	}

	mo_bool JSStackBuffer::Update(JSCallFrame* position)
	{
		if(position < mBegin || position > mEnd)
			return mo_false;

		mPosition = position;
		return mo_true;
	}

	void JSStackBuffer::Shrink(mo_uint32 size)
	{
		JSCallFrame* nextPos = mPosition - size;

		if(nextPos >= mBegin)
			mPosition = nextPos;
	}
}