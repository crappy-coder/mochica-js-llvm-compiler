#include "AST/Element.h"

namespace MoEnjin
{
	Element::Element(mo_int32 elision, Expression* expr)
		: mNext(0)
		, mExpression(expr)
		, mElision(elision) { }

	Element::Element(mo_int32 elision, Expression* expr, Element* next)
		: mNext(0)
		, mExpression(expr)
		, mElision(elision)
	{
		next->mNext = this;
	}
}