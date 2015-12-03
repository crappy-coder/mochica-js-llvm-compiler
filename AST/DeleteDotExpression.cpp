#include "AST/DeleteDotExpression.h"

namespace MoEnjin
{
	DeleteDotExpression::DeleteDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mBase(base)
		, mId(id) { }
}