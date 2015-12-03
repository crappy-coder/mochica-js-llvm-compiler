#include "AST/NotEqualExpression.h"

namespace MoEnjin
{
	NotEqualExpression::NotEqualExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_neq, rightHasAssignments) { }
}