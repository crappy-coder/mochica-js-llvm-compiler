#include "AST/ReturnStatement.h"

namespace MoEnjin
{
	ReturnStatement::ReturnStatement(mo_int32 lineNumber, Expression* value)
		: Statement(lineNumber)
		, mValue(value) { }
}