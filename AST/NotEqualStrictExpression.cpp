#include "AST/NotEqualStrictExpression.h"

namespace MoEnjin
{
	NotEqualStrictExpression::NotEqualStrictExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_nstricteq, rightHasAssignments) { }
}