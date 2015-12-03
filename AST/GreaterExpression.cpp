#include "AST/GreaterExpression.h"

namespace MoEnjin
{
	GreaterExpression::GreaterExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_greater, rightHasAssignments) { }
}