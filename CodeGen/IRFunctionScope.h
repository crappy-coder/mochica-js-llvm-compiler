#ifndef MOENJIN_CODEGEN_IRFUNCTIONSCOPE_H
#define MOENJIN_CODEGEN_IRFUNCTIONSCOPE_H

#include "Platform/Configuration.h"
#include "CodeGen/IRFunctionType.h"
#include "CodeGen/IRJumpDestination.h"
#include "CodeGen/IRValue.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/SmallVector.h>

namespace MoEnjin
{
	class IRFunctionScope
	{
		MO_NON_COPYABLE_CLASS(IRFunctionScope);

		friend class IREmitter;

		struct BreakContinueScope
		{
			IRJumpDestination continueDest;
			IRJumpDestination breakDest;

			BreakContinueScope(const IRJumpDestination& continueDest, const IRJumpDestination& breakDest)
				: continueDest(continueDest), breakDest(breakDest) { }

			BreakContinueScope(const BreakContinueScope& other)
				: continueDest(other.continueDest), breakDest(other.breakDest) { }
		};

		struct ExceptionCatchScope
		{
			UTF8String catchVariableName;
			IRValue exceptionValue;

			ExceptionCatchScope(const UTF8String& catchVariableName, const IRValue& exceptionValue)
				: catchVariableName(catchVariableName), exceptionValue(exceptionValue) { }

			ExceptionCatchScope(const ExceptionCatchScope& other)
				: catchVariableName(other.catchVariableName), exceptionValue(other.exceptionValue) { }
		};

		typedef llvm::SmallVector<BreakContinueScope, 8> BreakContinueScopeCollection;
		typedef llvm::SmallVector<ExceptionCatchScope, 8> ExceptionCatchScopeCollection;
		typedef std::vector<IRSwitchInstruction> IRSwitchInstructionCollection;
		typedef std::vector<IRValue> IRValueCollection;

		public:
			IRFunctionScope(IRFunctionType type, const IRFunction& function, const IRBasicBlock& entryBlock, const IRJumpDestination& returnDest);

			IRFunctionType GetType() const { return mType; }
			IRFunction GetFunction() const { return mFunction; }
			IRBasicBlock GetEntryBlock() const { return mEntryBlock; }
			IRJumpDestination GetReturnDestination() const { return mReturnDest; }
			IRValue GetReturnValue() const { return mReturnValue; }
			IRValue GetExecutionContext() const { return mExecutionContext; }
			IRSwitchInstruction GetSwitch() const;
			const IRJumpDestination& GetContinueDestination() const;
			const IRJumpDestination& GetBreakDestination() const;
			llvm::IRBuilderBase::InsertPoint GetRestorePoint() const { return mRestorePoint; }
			IRInstruction GetAllocationInsertPoint() const { return mAllocationInsertPoint; }
			IRValue GetNormalCleanupDestSlot() const { return mNormalCleanupDestSlot; }
			IRBasicBlock GetUnreachableBlock() const { return mUnreachableBlock; }
			const UTF8String& GetExceptionCatchVariableName() const;
			IRValue GetExceptionValue() const;

			mo_uint32 GetReturnCount() const { return mReturnCount; }
			mo_uint32 GetReturnsWithValueCount() const { return mReturnsWithValueCount; }

			mo_bool HasExecutionContext() const { return !!mExecutionContext; }
			mo_bool InSwitch() const { return !mSwitchStack.empty(); }
			mo_bool InBreakContinueScope() const { return !mBreakContinueStack.empty(); }
			mo_bool InExceptionCatchScope() const { return !mExceptionCatchStack.empty(); }

			void SetExecutionContext(const IRValue& value) { mExecutionContext = value; }
			void SetRestorePoint(const llvm::IRBuilderBase::InsertPoint& value) { mRestorePoint = value; }
			void SetReturnCount(mo_uint32 value) { mReturnCount = value; }
			void SetReturnsWithValueCount(mo_uint32 value) { mReturnsWithValueCount = value; }
			void SetNormalCleanupDestSlot(const IRValue& value) { mNormalCleanupDestSlot = value; }
			void SetUnreachableBlock(const IRBasicBlock& value) { mUnreachableBlock = value; }

			void BeginSwitch(const IRSwitchInstruction& value);
			void EndSwitch();

			void BeginBreakContinueScope(const IRJumpDestination& continueDest, const IRJumpDestination& breakDest);
			void EndBreakContinueScope();

			void BeginExceptionCatchScope(const UTF8String& catchVariableName, const IRValue& exceptionValue);
			void EndExceptionCatchScope();

		private:
			IRFunctionType mType;
			IRFunction mFunction;
			IRBasicBlock mEntryBlock;
			IRJumpDestination mReturnDest;
			IRBasicBlock mUnreachableBlock;
			IRValue mReturnValue;
			IRValue mExecutionContext;
			IRInstruction mAllocationInsertPoint;
			IRValue mNormalCleanupDestSlot;
			IRSwitchInstructionCollection mSwitchStack;
			BreakContinueScopeCollection mBreakContinueStack;
			ExceptionCatchScopeCollection mExceptionCatchStack;
			llvm::IRBuilderBase::InsertPoint mRestorePoint;
			mo_uint32 mReturnCount;
			mo_uint32 mReturnsWithValueCount;
	};
}

#endif