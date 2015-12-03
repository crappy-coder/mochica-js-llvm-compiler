#include "AST/CommaExpression.h"

namespace MoEnjin
{
	CommaExpression::CommaExpression(mo_int32 lineNumber, Expression* expr1, Expression* expr2)
		: Expression(lineNumber)
	{
		mExpressions.push_back(expr1);
		mExpressions.push_back(expr2);
	}
}