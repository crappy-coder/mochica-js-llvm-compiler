#ifndef MOENJIN_AST_DELETEVALUEEXPRESSION_H
#define MOENJIN_AST_DELETEVALUEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class DeleteValueExpression : public Expression
	{
		public:
			DeleteValueExpression(mo_int32 lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_DELETE_VALUE; }

			Expression* GetExpression() const { return mExpression; }

		private:
			Expression* mExpression;
	};
}

#endif