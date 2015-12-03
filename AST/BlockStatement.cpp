#include "AST/BlockStatement.h"

namespace MoEnjin
{
	BlockStatement::BlockStatement(mo_int32 lineNumber, StatementList* statements)
		: Statement(lineNumber)
		, mStatements(statements) { }
}