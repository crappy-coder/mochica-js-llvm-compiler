#include "AST/PrePostResolveExpression.h"

namespace MoEnjin
{
	PrePostResolveExpression::PrePostResolveExpression(mo_int32 lineNumber, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber, ResultKind::Number())
		, ThrowableExpression(divot, startPos, endPos)
		, mId(id) { }
}