#include "AST/TypeOfResolveExpression.h"

namespace MoEnjin
{
	TypeOfResolveExpression::TypeOfResolveExpression(mo_int32 lineNumber, const UTF8String& id)
		: Expression(lineNumber, ResultKind::String())
		, mId(id) { }
}