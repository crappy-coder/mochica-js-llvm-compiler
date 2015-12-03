#ifndef MOENJIN_CODEGEN_IREXCEPTIONHANDLINGHAZARDS_H
#define MOENJIN_CODEGEN_IREXCEPTIONHANDLINGHAZARDS_H

#include "Platform/Configuration.h"
#include "CodeGen/IRTypes.h"
#include "CodeGen/IRValue.h"

#include <llvm/ADT/DenseSet.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/InlineAsm.h>

namespace MoEnjin
{
	class IRExceptionHandlingHazards
	{
		friend class IREmitter;

		public:
			IRExceptionHandlingHazards(IREmitter* emitter);

		private:
			llvm::FunctionType* GetASMFnType();

			void CollectLocals();
			void EmitHazardsInNewBlocks();
			void EmitReadHazard(IRBuilderDef &builder);
			void EmitWriteHazard();

		private:
			IREmitter* mEmitter;
			llvm::SmallVector<llvm::Value*, 20> mLocals;
			llvm::DenseSet<llvm::BasicBlock*> mBlocksBeforeTry;
			llvm::InlineAsm* mReadHazard;
			llvm::InlineAsm* mWriteHazard;
	};
}

#endif