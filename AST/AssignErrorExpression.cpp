#include "AST/AssignErrorExpression.h"

namespace MoEnjin
{
	AssignErrorExpression::AssignErrorExpression(mo_int32 lineNumber, Expression* left, Expression* right, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mLeft(left)
		, mRight(right)
		, mOp(op) { }
}