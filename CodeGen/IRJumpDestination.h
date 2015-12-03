#ifndef MOENJIN_CODEGEN_IRJUMPDESTINATION_H
#define MOENJIN_CODEGEN_IRJUMPDESTINATION_H

#include "Platform/Configuration.h"
#include "CodeGen/IRCleanup.h"

namespace MoEnjin
{
	struct IRJumpDestination
	{
		public:
			IRJumpDestination()
				: mBlock(0), mCleanup(), mIndex(0) { }

			IRJumpDestination(const IRBasicBlock& block, const IRCleanupRef& cleanup, mo_uint32 index)
				: mBlock(block), mCleanup(cleanup), mIndex(index) { }

			IRJumpDestination(const IRJumpDestination& other)
				: mBlock(other.mBlock), mCleanup(other.mCleanup), mIndex(other.mIndex) { }

			mo_bool IsValid() const { return !mBlock.IsNull(); }

			IRBasicBlock GetBlock() const { return mBlock; }
			IRCleanupRef GetCleanup() const { return mCleanup; }
			mo_uint32 GetIndex() const { return mIndex; }

			void SetCleanup(const IRCleanupRef& cleanup) { mCleanup = cleanup; }

		private:
			IRBasicBlock mBlock;
			IRCleanupRef mCleanup;
			mo_uint32 mIndex;
	};
}

#endif