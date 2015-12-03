#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	ThrowableBinaryExpression::ThrowableBinaryExpression(mo_int32 lineNumber, ResultKind kind, Expression* left, Expression* right, OpCodeID opcode, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, kind, left, right, opcode, rightHasAssignments) { }

	ThrowableBinaryExpression::ThrowableBinaryExpression(mo_int32 lineNumber, Expression* left, Expression* right, OpCodeID opcode, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, left, right, opcode, rightHasAssignments) { }
}