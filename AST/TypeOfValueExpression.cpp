#include "AST/TypeOfValueExpression.h"

namespace MoEnjin
{
	TypeOfValueExpression::TypeOfValueExpression(mo_int32 lineNumber, Expression* expression)
		: Expression(lineNumber, ResultKind::String())
		, mExpression(expression) { }
}