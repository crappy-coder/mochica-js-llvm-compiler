#include "AST/BitwiseNotExpression.h"

namespace MoEnjin
{
	BitwiseNotExpression::BitwiseNotExpression(mo_int32 lineNumber, Expression* expression)
		: Expression(lineNumber, ResultKind::ForBitOp())
		, mExpression(expression) { }
}