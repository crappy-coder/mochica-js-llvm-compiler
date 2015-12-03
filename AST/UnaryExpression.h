#ifndef MOENJIN_AST_UNARYEXPRESSION_H
#define MOENJIN_AST_UNARYEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/OpCodes.h"

namespace MoEnjin
{
	class UnaryExpression : public Expression
	{
		public:
			UnaryExpression(mo_int32 lineNumber, ResultKind kind, Expression* expression);

			virtual const NodeType GetType() const { return NODE_UNARY_OP; }

			Expression* GetExpression() const { return mExpression; }

		private:
			Expression* mExpression;
	};
}

#endif