#ifndef MOENJIN_AST_EXPRESSIONSTATEMENT_H
#define MOENJIN_AST_EXPRESSIONSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ExpressionStatement : public Statement
	{
		public:
			ExpressionStatement(mo_int32 lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_EXPR_STATEMENT; }

			Expression* GetExpression() const { return mExpression; }

		private:
			Expression* mExpression;
	};
}

#endif