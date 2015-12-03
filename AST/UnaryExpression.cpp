#include "AST/UnaryExpression.h"

namespace MoEnjin
{
	UnaryExpression::UnaryExpression(mo_int32 lineNumber, ResultKind kind, Expression* expression)
		: Expression(lineNumber, kind)
		, mExpression(expression) { }
}