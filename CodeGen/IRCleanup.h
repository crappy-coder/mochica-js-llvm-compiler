#ifndef MOENJIN_CODEGEN_IRCLEANUP_H
#define MOENJIN_CODEGEN_IRCLEANUP_H

#include "Platform/Configuration.h"
#include "CodeGen/IRConsts.h"
#include "CodeGen/IRValue.h"

#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>

namespace MoEnjin
{
	enum IRCleanupType
	{
		kIRCleanupTypeNormal						= 0x01,
		kIRCleanupTypeException						= 0x02,
		kIRCleanupTypeNormalAndException			= kIRCleanupTypeNormal | kIRCleanupTypeException
	};

	class IREmitter;
	class IRCleanup;
	class IRCleanupStack;
	class IRCleanupTask;

	struct IRCleanupRef
	{
		public:
			static IRCleanupRef Invalid() { return IRCleanupRef(0); }

		public:
			IRCleanupRef()
				: mStack(0), mPosition(-1) { }

			explicit 
			IRCleanupRef(IRCleanupStack* stack)
				: mStack(stack), mPosition(-1) { }

			IRCleanupRef(const IRCleanupStack* stack, mo_uint32 index)
				: mStack(stack), mPosition(index) { }

			IRCleanupRef(const IRCleanupRef& other)
				: mStack(other.mStack), mPosition(other.mPosition) { }

			mo_bool IsValid() const;
			mo_bool Encloses(const IRCleanupRef& other) const;
			mo_bool StrictlyEncloses(const IRCleanupRef& other) const;

			mo_int32 GetPosition() const { return mPosition; }
			IRCleanup* Get() const { return GetImpl(); }
			IRCleanup* operator->() const { return Get(); }
			IRCleanup& operator*() const { return *Get(); }

			friend mo_bool operator==(const IRCleanupRef& a, const IRCleanupRef& b) { return a.mPosition == b.mPosition; }
			friend mo_bool operator!=(const IRCleanupRef& a, const IRCleanupRef& b) { return a.mPosition != b.mPosition; }

		private:
			IRCleanup* GetImpl() const;

			const IRCleanupStack* mStack;
			mo_int32 mPosition;
	};

	class IRCleanup
	{
		typedef llvm::SmallPtrSet<llvm::BasicBlock*, 4> BranchCollection;
		typedef llvm::SmallVector<std::pair<llvm::BasicBlock*, llvm::ConstantInt*>, 4> BranchAfterCollection;

		public:
			IRCleanup(IRCleanupType type, IRCleanupTask* task, const IRCleanupRef& enclosingCleanup);
			~IRCleanup();

			IRCleanupTask* GetTask() const { return mTask; }
			IRCleanupType GetType() const { return mType; }

			IRCleanupRef GetEnclosingCleanup() const { return mEnclosingCleanup; }
			IRBasicBlock GetEntryBlock() const { return mEntryBlock; }
			
			void SetEntryBlock(const IRBasicBlock& block) { mEntryBlock = block; }

			mo_bool HasBranches() const { return !mBranches.empty(); }
			mo_bool HasBranchThoughs() const { return (mBranchAfters.size() != mBranches.size()); }

			mo_uint32 GetBranchAfterCount() const { return mBranchAfters.size(); }

			llvm::BasicBlock* GetBranchAfterBlock(mo_uint32 i) const
			{
				MO_ASSERT(i < GetBranchAfterCount());
				return mBranchAfters[i].first;
			}

			llvm::ConstantInt* GetBranchAfterIndex(mo_uint32 i) const
			{
				MO_ASSERT(i < GetBranchAfterCount());
				return mBranchAfters[i].second;
			}

			void AddBranchAfter(llvm::ConstantInt* index, llvm::BasicBlock* block)
			{
				if(mBranches.insert(block))
					mBranchAfters.push_back(std::make_pair(block, index));
			}

			mo_bool AddBranchThrough(llvm::BasicBlock* block)
			{
				return mBranches.insert(block);
			}

		private:
			IRCleanupTask* mTask;
			IRCleanupType mType;
			IRCleanupRef mEnclosingCleanup;
			IRBasicBlock mEntryBlock;
			BranchCollection mBranches;
			BranchAfterCollection mBranchAfters;
	};

	class IRCleanupTask
	{
		public:
			virtual ~IRCleanupTask() { }
			virtual const char* GetEntryBlockName() const { return IR_BLOCK_NAME("Cleanup"); }
			virtual void Emit(IREmitter* emitter) = 0;
	};
}

#endif