#include "AST/ReadModifyResolveExpression.h"

namespace MoEnjin
{
	ReadModifyResolveExpression::ReadModifyResolveExpression(mo_int32 lineNumber, const UTF8String& id, Operator op, Expression* right, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mId(id)
		, mOp(op)
		, mRight(right)
		, mRightHasAssignments(rightHasAssignments) { }
}