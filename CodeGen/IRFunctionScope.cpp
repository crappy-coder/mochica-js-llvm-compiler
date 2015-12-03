#include "CodeGen/IRFunctionScope.h"
#include "CodeGen/IRTypeBuilder.h"
#include <llvm/IR/Instructions.h>

namespace MoEnjin
{
	IRFunctionScope::IRFunctionScope(IRFunctionType type, const IRFunction& function, const IRBasicBlock& entryBlock, const IRJumpDestination& returnDest)
		: mType(type)
		, mFunction(function)
		, mEntryBlock(entryBlock)
		, mReturnDest(returnDest)
		, mReturnCount(0)
		, mReturnsWithValueCount(0)
		, mNormalCleanupDestSlot(0)
		, mUnreachableBlock(0)
	{ 
		mReturnValue = function->getReturnType()->isVoidTy() ? IRValue::Null : new llvm::AllocaInst(function->getReturnType(), 0, "retval", mEntryBlock);

		mAllocationInsertPoint = new llvm::BitCastInst(
			llvm::UndefValue::get(IRTypeBuilder::GetType(IRInt32Ty)),
			IRTypeBuilder::GetType(IRInt32Ty), "", entryBlock.Get());

		mAllocationInsertPoint->setName("allocation_marker");
	}

	IRSwitchInstruction IRFunctionScope::GetSwitch() const
	{
		MO_ASSERT(!mSwitchStack.empty() && "GetSwitch: Not in a switch block.");

		return mSwitchStack.back();
	}

	const IRJumpDestination& IRFunctionScope::GetContinueDestination() const
	{
		MO_ASSERT(!mBreakContinueStack.empty() && "GetContinueDestination: Not in a break/continue scope.");

		return mBreakContinueStack.back().continueDest;
	}

	const IRJumpDestination& IRFunctionScope::GetBreakDestination() const
	{
		MO_ASSERT(!mBreakContinueStack.empty() && "GetBreakDestination: Not in a break/continue scope.");

		return mBreakContinueStack.back().breakDest;
	}

	const UTF8String& IRFunctionScope::GetExceptionCatchVariableName() const
	{
		MO_ASSERT(!mExceptionCatchStack.empty() && "GetExceptionCatchVariableName: Not in a catch scope.");

		return mExceptionCatchStack.back().catchVariableName;
	}

	IRValue IRFunctionScope::GetExceptionValue() const
	{
		MO_ASSERT(!mExceptionCatchStack.empty() && "GetExceptionValue: Not in a catch scope.");

		return mExceptionCatchStack.back().exceptionValue;
	}

	void IRFunctionScope::BeginSwitch(const IRSwitchInstruction& value)
	{
		mSwitchStack.push_back(value);
	}

	void IRFunctionScope::EndSwitch()
	{
		MO_ASSERT(!mSwitchStack.empty() && "EndSwitch: Cannot end switch block when not in one.");

		mSwitchStack.pop_back();
	}

	void IRFunctionScope::BeginBreakContinueScope(const IRJumpDestination& continueDest, const IRJumpDestination& breakDest)
	{
		mBreakContinueStack.push_back(BreakContinueScope(continueDest, breakDest));
	}

	void IRFunctionScope::EndBreakContinueScope()
	{
		MO_ASSERT(!mBreakContinueStack.empty() && "EndBreakContinueScope: Call was made to end break/continue scope but no scopes exist.");

		mBreakContinueStack.pop_back();
	}

	void IRFunctionScope::BeginExceptionCatchScope(const UTF8String& catchVariableName, const IRValue& exceptionValue)
	{
		mExceptionCatchStack.push_back(ExceptionCatchScope(catchVariableName, exceptionValue));
	}

	void IRFunctionScope::EndExceptionCatchScope()
	{
		MO_ASSERT(!mExceptionCatchStack.empty() && "EndExceptionCatchScope: No exceptions are in the catch stack.");

		mExceptionCatchStack.pop_back();
	}
}