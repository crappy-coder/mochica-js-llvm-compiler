#include "AST/LeftShiftExpression.h"

namespace MoEnjin
{
	LeftShiftExpression::LeftShiftExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::ForBitOp(), left, right, op_lshift, rightHasAssignments) { }
}