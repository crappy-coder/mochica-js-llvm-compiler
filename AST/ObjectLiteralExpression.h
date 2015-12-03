#ifndef MOENJIN_AST_OBJECTLITERALEXPRESSION_H
#define MOENJIN_AST_OBJECTLITERALEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/PropertyListNode.h"

namespace MoEnjin
{
	class ObjectLiteralExpression : public Expression
	{
		public:
			ObjectLiteralExpression(int lineNumber);
			ObjectLiteralExpression(int lineNumber, PropertyListNode* propList);

			virtual const NodeType GetType() const { return NODE_OBJECT_LITERAL; }

		private:
			PropertyListNode* mPropList;
	};
}

#endif