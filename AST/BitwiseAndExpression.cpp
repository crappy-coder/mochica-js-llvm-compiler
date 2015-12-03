#include "AST/BitwiseAndExpression.h"

namespace MoEnjin
{
	BitwiseAndExpression::BitwiseAndExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, ResultKind::ForBitOp(), left, right, op_bitand, rightHasAssignments) { }
}