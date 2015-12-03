#include "AST/BooleanExpression.h"

namespace MoEnjin
{
	BooleanExpression::BooleanExpression(mo_int32 lineNumber, mo_bool value)
		: Expression(lineNumber, ResultKind::Boolean())
		, mValue(value) { }
}