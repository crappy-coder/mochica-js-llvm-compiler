#include "AST/DeleteBracketExpression.h"

namespace MoEnjin
{
	DeleteBracketExpression::DeleteBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mBase(base)
		, mSubscript(subscript) { }
}