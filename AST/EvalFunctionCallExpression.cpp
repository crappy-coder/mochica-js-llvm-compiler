#include "AST/EvalFunctionCallExpression.h"

namespace MoEnjin
{
	EvalFunctionCallExpression::EvalFunctionCallExpression(mo_int32 lineNumber, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableExpression(divot, startPos, endPos)
		, mArgs(args) { }
}