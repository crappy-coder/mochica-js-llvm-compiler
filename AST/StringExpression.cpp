#include "AST/StringExpression.h"

namespace MoEnjin
{
	StringExpression::StringExpression(mo_int32 lineNumber, const UTF8String& value)
		: Expression(lineNumber, ResultKind::String())
		, mValue(value) { }
}