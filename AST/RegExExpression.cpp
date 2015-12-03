#include "AST/RegExExpression.h"

namespace MoEnjin
{
	RegExExpression::RegExExpression(int lineNumber, const UTF8String& pattern, const UTF8String& flags)
		: Expression(lineNumber)
		, mPattern(pattern)
		, mFlags(flags) { }
}