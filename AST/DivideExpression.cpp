#include "AST/DivideExpression.h"

namespace MoEnjin
{
	DivideExpression::DivideExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Number(), left, right, op_div, rightHasAssignments) { }
}