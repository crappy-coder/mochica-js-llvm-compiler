#include "AST/UnaryPlusExpression.h"

namespace MoEnjin
{
	UnaryPlusExpression::UnaryPlusExpression(mo_int32 lineNumber, Expression* expression)
		: UnaryExpression(lineNumber, ResultKind::Number(), expression) { }
}