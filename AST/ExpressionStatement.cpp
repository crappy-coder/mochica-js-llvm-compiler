#include "AST/ExpressionStatement.h"

namespace MoEnjin
{
	ExpressionStatement::ExpressionStatement(mo_int32 lineNumber, Expression* expression)
		: Statement(lineNumber)
		, mExpression(expression) { }
}