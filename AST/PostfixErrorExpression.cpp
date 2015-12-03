#include "AST/PostfixErrorExpression.h"

namespace MoEnjin
{
	PostfixErrorExpression::PostfixErrorExpression(mo_int32 lineNumber, Expression* expression, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableSubExpression(divot, startPos, endPos)
		, mExpression(expression)
		, mOp(op) { }
}