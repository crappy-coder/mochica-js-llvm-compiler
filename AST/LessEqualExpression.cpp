#include "AST/LessEqualExpression.h"

namespace MoEnjin
{
	LessEqualExpression::LessEqualExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Boolean(), left, right, op_lesseq, rightHasAssignments) { }
}