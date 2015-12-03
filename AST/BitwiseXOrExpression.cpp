#include "AST/BitwiseXOrExpression.h"

namespace MoEnjin
{
	BitwiseXOrExpression::BitwiseXOrExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, ResultKind::ForBitOp(), left, right, op_bitxor, rightHasAssignments) { }
}