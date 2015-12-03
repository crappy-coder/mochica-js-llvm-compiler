#include "AST/PostfixBracketExpression.h"

namespace MoEnjin
{
	PostfixBracketExpression::PostfixBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableSubExpression(divot, startPos, endPos)
		, mBase(base)
		, mSubscript(subscript)
		, mOp(op) { }
}