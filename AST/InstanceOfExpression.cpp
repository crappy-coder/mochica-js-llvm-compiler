#include "AST/InstanceOfExpression.h"

namespace MoEnjin
{
	InstanceOfExpression::InstanceOfExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_instanceof, rightHasAssignments) { }
}