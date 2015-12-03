#ifndef MOENJIN_AST_THROWSTATEMENT_H
#define MOENJIN_AST_THROWSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/ThrowableExpression.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ThrowStatement : public Statement, public ThrowableExpression
	{
		public:
			ThrowStatement(int lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_THROW; }

			Expression* GetExpression() const { return mExpression; }

		private:
			Expression* mExpression;
	};
}

#endif