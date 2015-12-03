#include "AST/BreakStatement.h"

namespace MoEnjin
{
	BreakStatement::BreakStatement(int lineNumber)
		: Statement(lineNumber) { }

	BreakStatement::BreakStatement(int lineNumber, const UTF8String& id)
		: Statement(lineNumber)
		, mId(id) { }
}