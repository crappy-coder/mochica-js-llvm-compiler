#include "AST/WhileStatement.h"

namespace MoEnjin
{
	WhileStatement::WhileStatement(mo_int32 lineNumber, Expression* expression, Statement* statement)
		: Statement(lineNumber)
		, mStatement(statement)
		, mExpression(expression) { }
}