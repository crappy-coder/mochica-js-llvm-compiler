#include "AST/ProgramNode.h"

#include <llvm/Support/PathV2.h>

namespace MoEnjin
{
	ProgramNode::ProgramNode(GlobalContext* globalContext, mo_int32 lineNumber, const SourceCode& source, StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount)
		: ScopedStatement(globalContext, lineNumber, source, statements, variables, functions, capturedVariables, features, constantCount) { }

	UTF8String ProgramNode::GetSourceScriptName() const
	{
		UTF8String sourceUrl = GetSource().GetUrl();
		UTF8String scriptName = UTF8String(llvm::sys::path::stem(sourceUrl.GetChars()).str());

		return scriptName;
	}
}