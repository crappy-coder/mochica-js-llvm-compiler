#include "AST/ValueFunctionCallExpression.h"

namespace MoEnjin
{
	ValueFunctionCallExpression::ValueFunctionCallExpression(mo_int32 lineNumber, Expression* expression, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mExpression(expression)
		, mArgs(args) { }
}