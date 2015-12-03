#include "AST/ConstDeclExpression.h"

namespace MoEnjin
{
	ConstDeclExpression::ConstDeclExpression(mo_int32 lineNumber, const UTF8String& id, Expression* initExpr, ConstDeclExpression* next)
		: Expression(lineNumber)
		, mId(id)
		, mInitExpr(initExpr)
		, mNext(0)
	{
		if(next)
			next->mNext = this;
	}
}