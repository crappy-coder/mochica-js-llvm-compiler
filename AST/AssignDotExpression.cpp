#include "AST/AssignDotExpression.h"

namespace MoEnjin
{
	AssignDotExpression::AssignDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, Expression* right, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mBase(base)
		, mId(id)
		, mRight(right)
		, mRightHasAssignments(rightHasAssignments) { }
}