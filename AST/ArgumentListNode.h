#ifndef MOENJIN_AST_ARGUMENTLISTNODE_H
#define MOENJIN_AST_ARGUMENTLISTNODE_H

#include "Platform/Configuration.h"
#include "AST/Node.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ArgumentListNode : public Node
	{
		public:
			ArgumentListNode(mo_int32 lineNumber, Expression* expression);
			ArgumentListNode(mo_int32 lineNumber, ArgumentListNode* next, Expression* expression);

			virtual const NodeType GetType() const { return NODE_ARGUMENT_LIST; }

			Expression* GetExpression() const { return mExpression; }
			ArgumentListNode* GetNext() const { return mNext; }

		private:
			Expression* mExpression;
			ArgumentListNode* mNext;
	};
}

#endif