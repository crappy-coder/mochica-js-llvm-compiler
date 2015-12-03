#include "AST/LessExpression.h"

namespace MoEnjin
{
	LessExpression::LessExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_less, rightHasAssignments) { }
}