#include "AST/ReadModifyDotExpression.h"

namespace MoEnjin
{
	ReadModifyDotExpression::ReadModifyDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, Operator op, Expression* right, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableSubExpression(divot, startPos, endPos)
		, mBase(base)
		, mId(id)
		, mOp(op)
		, mRight(right)
		, mRightHasAssignments(rightHasAssignments) { }
}