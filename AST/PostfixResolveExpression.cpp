#include "AST/PostfixResolveExpression.h"

namespace MoEnjin
{
	PostfixResolveExpression::PostfixResolveExpression(mo_int32 lineNumber, Operator op, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: PrePostResolveExpression(lineNumber, id, divot, startPos, endPos)
		, mOp(op) { }
}