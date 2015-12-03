#include "AST/VoidExpression.h"

namespace MoEnjin
{
	VoidExpression::VoidExpression(mo_int32 lineNumber, Expression* expression)
		: Expression(lineNumber)
		, mExpression(expression) { }
}