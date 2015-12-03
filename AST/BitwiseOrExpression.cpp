#include "AST/BitwiseOrExpression.h"

namespace MoEnjin
{
	BitwiseOrExpression::BitwiseOrExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, ResultKind::ForBitOp(), left, right, op_bitor, rightHasAssignments) { }
}