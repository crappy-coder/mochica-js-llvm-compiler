#ifndef MOENJIN_CODEGEN_IRAUTOCLEANUPSCOPE_H
#define MOENJIN_CODEGEN_IRAUTOCLEANUPSCOPE_H

#include "Platform/Configuration.h"
#include "CodeGen/IRCleanup.h"
#include "CodeGen/IREmitter.h"

namespace MoEnjin
{
	class IRAutoCleanupScope
	{
		MO_NON_COPYABLE_CLASS(IRAutoCleanupScope);

		public:
			explicit IRAutoCleanupScope(IREmitter* emitter)
				: mEmitter(emitter), mNeedsCleanup(mo_true) 
			{
				mCleanupStackDepth = mEmitter->GetCleanupStackDepth();
			}

			~IRAutoCleanupScope()
			{
				if(mNeedsCleanup)
					mEmitter->PopCleanupBlocks(mCleanupStackDepth);
			}

			mo_bool IsCleanupNeeded() const { return mEmitter->GetCleanupStackDepth() != mCleanupStackDepth; }

			void CleanupNow()
			{
				MO_ASSERT(mNeedsCleanup && "cleanup has already been run");

				mEmitter->PopCleanupBlocks(mCleanupStackDepth);
				mNeedsCleanup = mo_false;
			}

		private:
			IREmitter* mEmitter;
			IRCleanupRef mCleanupStackDepth;
			mo_bool mNeedsCleanup;
	};
}

#endif