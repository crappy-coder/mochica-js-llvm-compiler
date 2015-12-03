#include "AST/ConstStatement.h"

namespace MoEnjin
{
	ConstStatement::ConstStatement(mo_int32 lineNumber, ConstDeclExpression* expr)
		: Statement(lineNumber)
		, mExpression(expr) { }
}