#include "AST/SubtractExpression.h"

namespace MoEnjin
{
	SubtractExpression::SubtractExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Number(), left, right, op_sub, rightHasAssignments) { }
}