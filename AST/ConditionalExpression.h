#ifndef MOENJIN_AST_CONDITIONALEXPRESSION_H
#define MOENJIN_AST_CONDITIONALEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ConditionalExpression : public Expression
	{
		public:
			ConditionalExpression(mo_int32 lineNumber, Expression* condition, Expression* left, Expression* right);

			virtual const NodeType GetType() const { return NODE_CONDITIONAL; }

			Expression* GetCondition() const { return mCondition; }
			Expression* GetLeft() const { return mLeft; }
			Expression* GetRight() const { return mRight; }

		private:
			Expression* mCondition;
			Expression* mLeft;
			Expression* mRight;
	};
}

#endif