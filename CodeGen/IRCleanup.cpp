#include "CodeGen/IRCleanup.h"
#include "CodeGen/IRCleanupStack.h"

namespace MoEnjin
{
	IRCleanup::IRCleanup(IRCleanupType type, IRCleanupTask* task, const IRCleanupRef& enclosingCleanup)
		: mType(type)
		, mEnclosingCleanup(enclosingCleanup)
		, mEntryBlock(0)
		, mTask(task) {}

	IRCleanup::~IRCleanup()
	{

	}

	mo_bool IRCleanupRef::IsValid() const
	{
		return (mPosition >= 0);
	}

	mo_bool IRCleanupRef::Encloses(const IRCleanupRef& other) const
	{
		return (mPosition <= other.mPosition);
	}

	mo_bool IRCleanupRef::StrictlyEncloses(const IRCleanupRef& other) const
	{
		return (mPosition < other.mPosition);
	}

	IRCleanup* IRCleanupRef::GetImpl() const
	{
		MO_ASSERT(IsValid() && "cleanup reference is invalid.");
		MO_ASSERT(mPosition <= mStack->begin().mPosition && "item no longer exists.");

		return mStack->GetAt(mPosition-1);
	}
}