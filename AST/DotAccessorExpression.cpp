#include "AST/DotAccessorExpression.h"

namespace MoEnjin
{
	DotAccessorExpression::DotAccessorExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id)
		: Expression(lineNumber)
		, mBase(base)
		, mId(id) { }
}