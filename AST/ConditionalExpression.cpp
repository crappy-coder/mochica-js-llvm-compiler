#include "AST/ConditionalExpression.h"

namespace MoEnjin
{
	ConditionalExpression::ConditionalExpression(mo_int32 lineNumber, Expression* condition, Expression* left, Expression* right)
		: Expression(lineNumber)
		, mCondition(condition)
		, mLeft(left)
		, mRight(right) { }
		
}