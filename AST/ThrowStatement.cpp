#include "AST/ThrowStatement.h"

namespace MoEnjin
{
	ThrowStatement::ThrowStatement(int lineNumber, Expression* expression)
		: Statement(lineNumber)
		, mExpression(expression) { }
}