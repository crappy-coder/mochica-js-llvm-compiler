#include "AST/DeleteValueExpression.h"

namespace MoEnjin
{
	DeleteValueExpression::DeleteValueExpression(mo_int32 lineNumber, Expression* expression)
		: Expression(lineNumber)
		, mExpression(expression) { }
}