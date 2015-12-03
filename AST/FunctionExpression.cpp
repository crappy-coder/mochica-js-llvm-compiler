#include "AST/FunctionExpression.h"
#include "AST/FunctionNode.h"
#include "AST/ParameterNode.h"

namespace MoEnjin
{
	FunctionExpression::FunctionExpression(mo_int32 lineNumber, const UTF8String& id, FunctionNode* function, const SourceCode& source, ParameterNode* parameter)
		: Expression(lineNumber)
		, mFunction(function)
	{
		function->FinishParsing(source, parameter, id, NODE_FUNCTION_EXPR);
	}
}