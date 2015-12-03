#include "AST/FunctionDeclStatement.h"
#include "AST/FunctionNode.h"
#include "AST/ParameterNode.h"

namespace MoEnjin
{
	FunctionDeclStatement::FunctionDeclStatement(int lineNumber, const UTF8String& id, FunctionNode* function, const SourceCode& source, ParameterNode* parameter)
		: Statement(lineNumber)
		, mFunction(function)
	{
		mFunction->FinishParsing(source, parameter, id, NODE_FUNCTION_DECL);
	}
}