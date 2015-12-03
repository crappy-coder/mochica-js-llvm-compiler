#include "AST/PrefixDotExpression.h"

namespace MoEnjin
{
	PrefixDotExpression::PrefixDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowablePrefixedSubExpression(divot, startPos, endPos)
		, mBase(base)
		, mOp(op)
		, mId(id) { }
}