#include "AST/GreaterEqualExpression.h"

namespace MoEnjin
{
	GreaterEqualExpression::GreaterEqualExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_greatereq, rightHasAssignments) { }
}