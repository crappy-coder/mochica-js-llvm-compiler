#include "AST/ResolveFunctionCallExpression.h"

namespace MoEnjin
{
	ResolveFunctionCallExpression::ResolveFunctionCallExpression(mo_int32 lineNumber, const UTF8String& id, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mId(id)
		, mArgs(args) { }
}