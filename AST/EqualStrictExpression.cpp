#include "AST/EqualStrictExpression.h"

namespace MoEnjin
{
	EqualStrictExpression::EqualStrictExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_stricteq, rightHasAssignments) { }
}