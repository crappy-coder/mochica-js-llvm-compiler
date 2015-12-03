#ifndef MOENJIN_CODEGEN_IRPERFORMFINALLYCLEANUP_H
#define MOENJIN_CODEGEN_IRPERFORMFINALLYCLEANUP_H

#include "Platform/Configuration.h"
#include "AST/TryStatement.h"
#include "CodeGen/IRCleanup.h"
#include "CodeGen/IREmitter.h"
#include "CodeGen/IRTypes.h"

namespace MoEnjin
{
	struct IRPerformFinallyCleanup : IRCleanupTask
	{
		const TryStatement* mTryStatement;
		llvm::Value* mCanExitTryVar;
		llvm::Value* mExceptionData;

		IRPerformFinallyCleanup(const TryStatement* tryStatement, llvm::Value* canExitTryVar, llvm::Value* exceptionData)
			: mTryStatement(tryStatement)
			, mCanExitTryVar(canExitTryVar)
			, mExceptionData(exceptionData) { }

		const char* GetEntryBlockName() const { return IR_BLOCK_NAME("FinallyCleanup"); }

		void Emit(IREmitter* emitter)
		{
			IRBasicBlock tryEndBlock = emitter->NewBasicBlock(IR_BLOCK_NAME("TryExit"));
			IRBasicBlock finallyBeginBlock = emitter->NewBasicBlock(IR_BLOCK_NAME("FinallyEnter"));
			IRValue canExitTryValue = emitter->EmitLoad(mCanExitTryVar, IR_INTERNAL_VAR_NAME("can_exit_try_value"));

			// branch to the try exit block if the canExitTry bit has been set, 
			// otherwise begin the finally phase
			emitter->EmitBranch(canExitTryValue, tryEndBlock, finallyBeginBlock);

			// emit the try end block and call the exit function
			emitter->EmitBasicBlock(tryEndBlock);
			emitter->EmitCallToExceptionTryExit(mExceptionData);

			// emit the finally begin block, this has to always happen regardless
			// if we actually have a finally block in the try statement or not
			emitter->EmitBasicBlock(finallyBeginBlock);

			// emit the finally statement block, if there is one
			if(mTryStatement->HasFinallyBlock())
			{
				// save the current cleanup destination in case there is control
				// flow inside the finally statement
				IRValue cleanupDestValue = emitter->EmitLoad(emitter->GetNormalCleanupDestSlot());

				// emit the statement
				emitter->EmitStatement(mTryStatement->GetFinallyBlock());

				// make sure to store the current cleanup destination
				if(emitter->InBasicBlock())
				{

					emitter->EmitStore(emitter->GetNormalCleanupDestSlot().Get(), cleanupDestValue.Get());
					return;
				}

				// we must always have a block at the end of a cleanup
				emitter->EnsureBasicBlock(IR_BLOCK_NAME("NO_BASIC_BLOCK_ERROR__IN__IRPerformFinallyCleanup"));
			}
		}
	};
}

#endif