#include "CodeGen/IRExceptionHandlingHazards.h"
#include "CodeGen/IREmitter.h"
#include "CodeGen/IRFunctionScope.h"
#include "Platform/LLVM.h"

#include <llvm/Support/CallSite.h>

namespace MoEnjin
{
	// Create the fragile-ABI read and write hazards based on the current
	// state of the function, which is presumed to be immediately prior
	// to a 'try' block.  These hazards are used to maintain correct
	// semantics in the face of optimization and the fragile ABI's
	// cavalier use of setjmp/longjmp.
	IRExceptionHandlingHazards::IRExceptionHandlingHazards(IREmitter* emitter)
		: mEmitter(emitter)
	{
		CollectLocals();

		if(mLocals.empty())
			return;

		for(llvm::Function::iterator I = emitter->GetFunction()->begin(), E = emitter->GetFunction()->end(); I != E; ++I)
			mBlocksBeforeTry.insert(&*I);

		llvm::FunctionType* asmFnTy = GetASMFnType();

		// Create a read hazard for the allocas.  This inhibits dead-store
		// optimizations and forces the values to memory.  This hazard is
		// inserted before any 'throwing' calls in the protected scope to
		// reflect the possibility that the variables might be read from the
		// catch block if the call throws.
		{
			mo_utf8string constraint;

			for(mo_uint32 I = 0, E = mLocals.size(); I != E; ++I)
			{
				if(I)
					constraint += ',';

				constraint += "*m";
			}

			mReadHazard = llvm::InlineAsm::get(asmFnTy, "", constraint, mo_true, mo_false);
		}

		// Create a write hazard for the allocas.  This inhibits folding
		// loads across the hazard.  This hazard is inserted at the
		// beginning of the catch path to reflect the possibility that the
		// variables might have been written within the protected scope.
		{
			mo_utf8string constraint;

			for(mo_uint32 I = 0, E = mLocals.size(); I != E; ++I)
			{
				if(I)
					constraint += ',';

				constraint += "=*m";
			}

			mWriteHazard = llvm::InlineAsm::get(asmFnTy, "", constraint, mo_true, mo_false);
		}
	}

	llvm::FunctionType* IRExceptionHandlingHazards::GetASMFnType()
	{
		llvm::SmallVector<llvm::Type*, 16> localTypes(mLocals.size());

		for(mo_uint32 i = 0, e = mLocals.size(); i != e; ++i)
			localTypes[i] = mLocals[i]->getType();

		return llvm::FunctionType::get(IRTypeBuilder::GetType(IRVoidTy), localTypes, mo_false);
	}

	static void AddIfPresent(llvm::DenseSet<llvm::Value*>& S, llvm::Value* V) { if(V) S.insert(V); }

	void IRExceptionHandlingHazards::CollectLocals()
	{
		IRFunctionScope* curFnScope = mEmitter->GetFunctionScope();
		llvm::DenseSet<llvm::Value*> allocasToIgnore;

		AddIfPresent(allocasToIgnore, curFnScope->GetReturnValue().Get());
		//AddIfPresent(allocasToIgnore, curFnScope->GetNormalCleanupDest().Get());

		llvm::BasicBlock& entry = curFnScope->GetFunction()->getEntryBlock();

		for(llvm::BasicBlock::iterator I = entry.begin(), E = entry.end(); I != E; ++I)
		{
			if(llvm::isa<llvm::AllocaInst>(*I) && !allocasToIgnore.count(&*I))
				mLocals.push_back(&*I);
		}
	}

	void IRExceptionHandlingHazards::EmitHazardsInNewBlocks()
	{
		if(mLocals.empty())
			return;

		IRBuilderDef builder(mEmitter->GetContext());
		llvm::Function* curFn = mEmitter->GetFunction();

		for(llvm::Function::iterator FI = curFn->begin(), FE = curFn->end(); FI != FE; ++FI)
		{
			llvm::BasicBlock& block = *FI;

			if(mBlocksBeforeTry.count(&block))
				continue;

			for(llvm::BasicBlock::iterator BI = block.begin(), BE = block.end(); BI != BE; ++BI)
			{
				llvm::Instruction& I = *BI;

				if(!llvm::isa<llvm::CallInst>(I) && !llvm::isa<llvm::InvokeInst>(I))
					continue;

				if(llvm::isa<llvm::IntrinsicInst>(I))
					continue;

				llvm::CallSite CS(&I);

				if(CS.doesNotThrow())
					continue;

				builder.SetInsertPoint(&block, BI);
				EmitReadHazard(builder);
			}
		}
	}

	void IRExceptionHandlingHazards::EmitReadHazard(IRBuilderDef &builder)
	{
		MO_ASSERT(!mLocals.empty());

		llvm::CallInst* call = builder.CreateCall(mReadHazard, mLocals);
		call->setDoesNotThrow();
	}

	void IRExceptionHandlingHazards::EmitWriteHazard()
	{
		if(mLocals.empty())
			return;

		mEmitter->EmitCallWithAttrs(mWriteHazard, mLocals, kIRFunctionAttributeNoUnwind);
	}

}