#include "AST/LabelStatement.h"

namespace MoEnjin
{
	LabelStatement::LabelStatement(mo_int32 lineNumber, const UTF8String& name, Statement* statement)
		: Statement(lineNumber)
		, mName(name)
		, mStatement(statement) { }
}