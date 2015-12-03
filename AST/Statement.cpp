#include "AST/Statement.h"

namespace MoEnjin
{
	Statement::Statement(mo_int32 lineNumber)
		: Node(lineNumber)
		, mEndLine(0) { }

	void Statement::SetLocation(mo_int32 startLine, mo_int32 endLine)
	{
		mLineNumber = startLine;
		mEndLine = endLine;
	}
}