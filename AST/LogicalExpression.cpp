#include "AST/LogicalExpression.h"

namespace MoEnjin
{
	LogicalExpression::LogicalExpression(mo_int32 lineNumber, Expression* left, Expression* right, LogicalOperator op)
		: Expression(lineNumber, ResultKind::Boolean())
		, mLeft(left)
		, mRight(right)
		, mOp(op) { }
}