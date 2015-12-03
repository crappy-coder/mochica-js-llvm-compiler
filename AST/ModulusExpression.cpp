#include "AST/ModulusExpression.h"

namespace MoEnjin
{
	ModulusExpression::ModulusExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments)
		: BinaryExpression(lineNumber, ResultKind::Number(), left, right, op_mod, rightHasAssignments) { }
}