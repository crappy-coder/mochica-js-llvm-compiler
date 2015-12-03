#include "AST/LogicalNotExpression.h"

namespace MoEnjin
{
	LogicalNotExpression::LogicalNotExpression(mo_int32 lineNumber, Expression* expression)
		: UnaryExpression(lineNumber, ResultKind::Boolean(), expression) { }
}