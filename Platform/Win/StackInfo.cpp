#include "Platform/StackInfo.h"

namespace MoEnjin
{
	StackInfo::GrowDirection StackInfo::GetGrowthDirection() const
	{
		return mOrigin > mBounds ? kGrowDirectionDown : kGrowDirectionUp;
	}

	mo_bool StackInfo::CanGrow(mo_int32 minSize) const
	{
		return GetGrowthDirection() == kGrowDirectionDown ?
			GetPosition() >= GetGrowthLimit(minSize) : GetPosition() <= GetGrowthLimit(minSize);
	}

	void StackInfo::Initialize()
	{		
#if defined(MO_CPU_X86) && defined(MO_COMPILER_MSVC)
		NT_TIB* tib;

		__asm 
		{
			mov eax, FS:[18h]
			mov tib, eax
		}

		mOrigin = tib->StackBase;
		mBounds = tib->StackLimit;
		mSize = mo_cast<mo_utf8char*>(tib->StackBase) - mo_cast<mo_utf8char*>(tib->StackLimit);

#elif defined(MO_CPU_X86) && defined(MO_COMPILER_GCC)

#elif defined(MO_CPU_X86_64)

#else
#error This Windows build does not have a supported stack bounds implementation yet.
#endif

		
	}
}