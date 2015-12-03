#include "AST/EqualExpression.h"

namespace MoEnjin
{
	EqualExpression::EqualExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_eq, rightHasAssignments) { }
}