#include "AST/AssignBracketExpression.h"

namespace MoEnjin
{
	AssignBracketExpression::AssignBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, Expression* right, mo_bool subscriptHasAssignments, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mBase(base)
		, mSubscript(subscript)
		, mRight(right)
		, mSubscriptHasAssignments(subscriptHasAssignments)
		, mRightHasAssignments(rightHasAssignments) { }
}