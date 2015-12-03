#ifndef MOENJIN_CODEGEN_IRPERFORMCATCHCLEANUP_H
#define MOENJIN_CODEGEN_IRPERFORMCATCHCLEANUP_H

#include "Platform/Configuration.h"
#include "CodeGen/IRCleanup.h"
#include "CodeGen/IREmitter.h"
#include "CodeGen/IRTypes.h"

namespace MoEnjin
{
	struct IRPerformCatchCleanup : IRCleanupTask
	{
		IRPerformCatchCleanup() { }

		const char* GetEntryBlockName() const { return IR_BLOCK_NAME("CatchCleanup"); }

		void Emit(IREmitter* emitter)
		{
			emitter->EmitBasicBlock(
				emitter->NewBasicBlock(IR_BLOCK_NAME("CatchExit")));
			emitter->EmitCallToExceptionCatchExit();
		}
	};
}

#endif