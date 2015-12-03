#include "Runtime/JSStringRope.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JSStringRope::JSStringRope(JSGlobalObject* globalObject)
		: JSString(globalObject)
	{
		Initialize();
	}

	JSStringRope::JSStringRope(JSGlobalObject* globalObject, JSString* a, JSString* b)
		: JSString(globalObject)
	{
		Initialize();
		InitializeFibers(a, b);
	}

	JSStringRope::JSStringRope(JSGlobalObject* globalObject, JSString* a, JSString* b, JSString* c)
		: JSString(globalObject)
	{
		Initialize();
		InitializeFibers(a, b, c);
	}

	void JSStringRope::Initialize()
	{
		memset(mFibers, 0, sizeof(JSString*) * MaximumFiberCount);
	}

	void JSStringRope::InitializeFibers(JSString* a, JSString* b, JSString* c)
	{
		mLength = (a->GetLength() + b->GetLength() + (c ? c->GetLength() : 0));
		mFibers[0] = a;
		mFibers[1] = b;
		mFibers[2] = c;
	}

	JSString* JSStringRope::GetCharInRope(mo_unsigned index)
	{
		MO_ASSERT(IsRope());

		Resolve();

		if(GetGlobalObject()->HasException())
			return JSString::FromString(GetGlobalObject(), "");
		
		MO_ASSERT(index < mValue.GetLength());

		return JSString::FromSubstring(GetGlobalObject(), mValue, index);
	}

	void JSStringRope::Resolve() const
	{
		MO_ASSERT(IsRope());

		mValue = UTF8String::MakeEmpty();
		Resolve(this);

		MO_ASSERT(mValue.GetLength() == mLength);
		MO_ASSERT(!IsRope());
	}

	void JSStringRope::Resolve(const JSString* ropeOrFiber) const
	{
		if(ropeOrFiber != this && !ropeOrFiber->IsRope())
		{
			mValue.Append(ropeOrFiber->mValue);
			return;
		}

		const JSStringRope* rope = mo_cast<const JSStringRope*>(ropeOrFiber);

		for(mo_uint32 i = 0; i < MaximumFiberCount && rope->mFibers[i]; ++i)
		{
			// get the fiber slot
			const JSString* str = rope->mFibers[i];

			// clear the fiber slot, since we just want to resolve ourself
			// we must only clear it for ourself
			if(rope == this)
				rope->mFibers[i] = mo_null;

			// resolve it
			Resolve(str);
		}
	}

	void JSStringRope::UpdateFiber(JSString* fiber, mo_unsigned index)
	{
		// shrink the current length to account for removing
		// the fiber at index
		if(mFibers[index])
			mLength -= mFibers[index]->mLength;

		// update to new fiber
		mFibers[index] = fiber;

		// grow the current length now to account for the
		// newly added fiber
		mLength += fiber->mLength;
	}
}