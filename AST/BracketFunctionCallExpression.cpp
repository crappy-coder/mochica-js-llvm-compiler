#include "AST/BracketFunctionCallExpression.h"

namespace MoEnjin
{
	BracketFunctionCallExpression::BracketFunctionCallExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Expression(lineNumber)
		, ThrowableSubExpression(divot, startPos, endPos)
		, mBase(base)
		, mSubscript(subscript)
		, mArguments(args) { }
}