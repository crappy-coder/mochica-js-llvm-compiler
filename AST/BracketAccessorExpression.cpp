#include "AST/BracketAccessorExpression.h"

namespace MoEnjin
{
	BracketAccessorExpression::BracketAccessorExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, mo_bool subscriptHasAssignments)
		: Expression(lineNumber)
		, mBase(base)
		, mSubscript(subscript)
		, mSubscriptHasAssignments(subscriptHasAssignments) { }
}