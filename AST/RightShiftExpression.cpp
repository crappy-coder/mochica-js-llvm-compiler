#include "AST/RightShiftExpression.h"

namespace MoEnjin
{
	RightShiftExpression::RightShiftExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::ForBitOp(), left, right, op_rshift, rightHasAssignments) { }
}