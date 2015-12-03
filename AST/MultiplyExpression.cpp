#include "AST/MultiplyExpression.h"

namespace MoEnjin
{
	MultiplyExpression::MultiplyExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Number(), left, right, op_mul, rightHasAssignments) { }
}