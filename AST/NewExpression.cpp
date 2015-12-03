#include "AST/NewExpression.h"

namespace MoEnjin
{
	NewExpression::NewExpression(mo_int32 lineNumber, Expression* expression)
		: Expression(lineNumber)
		, mExpression(expression)
		, mArgs(0) { }

	NewExpression::NewExpression(mo_int32 lineNumber, Expression* expression, ArgumentsNode* args)
		: Expression(lineNumber)
		, mExpression(expression)
		, mArgs(args) { }
}