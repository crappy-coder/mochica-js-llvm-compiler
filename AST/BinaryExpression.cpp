#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	BinaryExpression::BinaryExpression(mo_int32 lineNumber, Expression* left, Expression* right, OpCodeID opcode, mo_bool rightHasAssignments)
		: Expression(lineNumber)
		, mLeftExpr(left)
		, mRightExpr(right)
		, mRightHasAssignments(rightHasAssignments)
		, mOpcode(opcode) { }

	BinaryExpression::BinaryExpression(mo_int32 lineNumber, ResultKind kind, Expression* left, Expression* right, OpCodeID opcode, mo_bool rightHasAssignments)
		: Expression(lineNumber, kind)
		, mLeftExpr(left)
		, mRightExpr(right)
		, mRightHasAssignments(rightHasAssignments)
		, mOpcode(opcode) { }
}