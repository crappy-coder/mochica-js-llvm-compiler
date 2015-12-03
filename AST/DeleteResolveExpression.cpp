#include "AST/DeleteResolveExpression.h"

namespace MoEnjin
{
	DeleteResolveExpression::DeleteResolveExpression(mo_int32 lineNumber, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mId(id) { }
}