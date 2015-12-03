#include "AST/DoWhileStatement.h"

namespace MoEnjin
{
	DoWhileStatement::DoWhileStatement(mo_int32 lineNumber, Statement* statement, Expression* expression)
		: Statement(lineNumber)
		, mStatement(statement)
		, mExpression(expression) {}
}