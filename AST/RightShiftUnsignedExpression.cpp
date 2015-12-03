#include "AST/RightShiftUnsignedExpression.h"

namespace MoEnjin
{
	RightShiftUnsignedExpression::RightShiftUnsignedExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Number(), left, right, op_urshift, rightHasAssignments) { }
}