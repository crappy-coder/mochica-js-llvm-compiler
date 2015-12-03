#include "AST/ReadModifyBracketExpression.h"

namespace MoEnjin
{
	ReadModifyBracketExpression::ReadModifyBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, Operator op, Expression* right, mo_bool subscriptHasAssignments, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableSubExpression(divot, startPos, endPos)
		, mBase(base)
		, mSubscript(subscript)
		, mRight(right)
		, mOp(op)
		, mSubscriptHasAssignments(subscriptHasAssignments)
		, mRightHasAssignments(rightHasAssignments) { }
}