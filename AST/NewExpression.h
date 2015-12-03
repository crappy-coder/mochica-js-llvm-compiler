#ifndef MOENJIN_AST_NEWEXPRESSION_H
#define MOENJIN_AST_NEWEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ArgumentsNode.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class NewExpression : public Expression, public ThrowableExpression
	{
		public:
			NewExpression(mo_int32 lineNumber, Expression* expression);
			NewExpression(mo_int32 lineNumber, Expression* expression, ArgumentsNode* args);

			virtual const NodeType GetType() const { return NODE_NEW; }

			Expression* GetExpression() const { return mExpression; }
			ArgumentsNode* GetArguments() const { return mArgs; }

		private:
			Expression* mExpression;
			ArgumentsNode* mArgs;
	};
}

#endif