#include "AST/ContinueStatement.h"

namespace MoEnjin
{
	ContinueStatement::ContinueStatement(GlobalContext* globalContext, int lineNumber)
		: Statement(lineNumber) { }

	ContinueStatement::ContinueStatement(int lineNumber, const UTF8String& id)
		: Statement(lineNumber)
		, mId(id) { }
}