#include "AST/NumberExpression.h"

namespace MoEnjin
{
	NumberExpression::NumberExpression(mo_int32 lineNumber, mo_double value)
		: Expression(lineNumber, ResultKind::Number())
		, mValue(value) { }
}