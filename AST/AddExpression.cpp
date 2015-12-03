#include "AST/AddExpression.h"

namespace MoEnjin
{
	AddExpression::AddExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::ForAdd(left->GetResultKind(), right->GetResultKind()), left, right, op_add, rightHasAssignments) { }
}