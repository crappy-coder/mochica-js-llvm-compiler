#ifndef MOENJIN_CODEGEN_IRCODEGENANNOTATIONWRITER_H
#define MOENJIN_CODEGEN_IRCODEGENANNOTATIONWRITER_H

#include "Platform/Configuration.h"
#include "Platform/LLVMDecls.h"
#include <llvm/Assembly/AssemblyAnnotationWriter.h>

namespace MoEnjin
{
	class IRCodeGenAnnotationWriter : public llvm::AssemblyAnnotationWriter
	{
		public:
			IRCodeGenAnnotationWriter();

			void printInfoComment(const llvm::Value &, llvm::formatted_raw_ostream &);
			void emitBasicBlockStartAnnot(const llvm::BasicBlock *, llvm::formatted_raw_ostream &);
	};
}

#endif