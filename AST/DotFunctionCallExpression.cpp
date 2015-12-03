#include "AST/DotFunctionCallExpression.h"

namespace MoEnjin
{
	DotFunctionCallExpression::DotFunctionCallExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableSubExpression(divot, startPos, endPos)
		, mBase(base)
		, mId(id)
		, mArguments(args) { }
}