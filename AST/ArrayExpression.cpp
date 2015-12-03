#include "AST/ArrayExpression.h"

namespace MoEnjin
{
	ArrayExpression::ArrayExpression(mo_int32 lineNumber, mo_int32 elision)
		: Expression(lineNumber)
		, mElision(elision)
		, mElement(0)
		, mOptional(mo_true) { }

	ArrayExpression::ArrayExpression(mo_int32 lineNumber, mo_int32 elision, Element* element)	
		: Expression(lineNumber)
		, mElision(elision)
		, mElement(element)
		, mOptional(mo_true) { }

	ArrayExpression::ArrayExpression(mo_int32 lineNumber, Element* element)
		: Expression(lineNumber)
		, mElision(0)
		, mElement(element)
		, mOptional(mo_false) { }
}