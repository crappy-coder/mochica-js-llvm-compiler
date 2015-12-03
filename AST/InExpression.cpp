#include "AST/InExpression.h"

namespace MoEnjin
{
	InExpression::InExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: ThrowableBinaryExpression(lineNumber, left, right, op_in, rightHasAssignments) { }
}