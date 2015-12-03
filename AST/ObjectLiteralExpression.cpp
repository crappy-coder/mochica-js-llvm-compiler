#include "AST/ObjectLiteralExpression.h"

namespace MoEnjin
{
	ObjectLiteralExpression::ObjectLiteralExpression(int lineNumber)
		: Expression(lineNumber)
		, mPropList(0) { }

	ObjectLiteralExpression::ObjectLiteralExpression(int lineNumber, PropertyListNode* propList)
		: Expression(lineNumber)
		, mPropList(propList) { }
}