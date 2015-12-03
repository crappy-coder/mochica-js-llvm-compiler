#include "CodeGen/IRCodeGenAnnotationWriter.h"
#include "Platform/LLVM.h"
#include <llvm/Support/FormattedStream.h>

namespace MoEnjin
{
	IRCodeGenAnnotationWriter::IRCodeGenAnnotationWriter() { }

	void IRCodeGenAnnotationWriter::printInfoComment(const llvm::Value& value, llvm::formatted_raw_ostream& output)
	{
		// TODO : hookup source comments
	}

	void IRCodeGenAnnotationWriter::emitBasicBlockStartAnnot(const llvm::BasicBlock* block, llvm::formatted_raw_ostream& output)
	{
		// TODO : hookup source comments
	}
}