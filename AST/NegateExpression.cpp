#include "AST/NegateExpression.h"

namespace MoEnjin
{
	NegateExpression::NegateExpression(mo_int32 lineNumber, Expression* expression)
		: UnaryExpression(lineNumber, ResultKind::Number(), expression) { }
}