#include "AST/AssignResolveExpression.h"

namespace MoEnjin
{
	AssignResolveExpression::AssignResolveExpression(mo_int32 lineNumber, const UTF8String& id, Expression* right, mo_bool rightHasAssignments)
		: Expression(lineNumber)
		, mId(id)
		, mRight(right)
		, mRightHasAssignments(rightHasAssignments) { }
}