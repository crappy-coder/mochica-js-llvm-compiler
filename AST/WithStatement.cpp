#include "AST/WithStatement.h"

namespace MoEnjin
{
	WithStatement::WithStatement(mo_int32 lineNumber, Expression* expression, Statement* statement, mo_uint32 divot, mo_uint32 expressionLength)
		: Statement(lineNumber)
		, mExpression(expression)
		, mStatement(statement)
		, mDivot(divot)
		, mExpressionLength(expressionLength) { }
}