#include "AST/VarStatement.h"

namespace MoEnjin
{
	VarStatement::VarStatement(mo_int32 lineNumber, Expression* expression)
		: Statement(lineNumber)
		, mExpression(expression) { }
}