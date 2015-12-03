#ifndef MOENJIN_AST_TYPEOFVALUEEXPRESSION_H
#define MOENJIN_AST_TYPEOFVALUEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class TypeOfValueExpression : public Expression
	{
		public:
			TypeOfValueExpression(mo_int32 lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_TYPEOF_VALUE; }

			Expression* GetExpression() const { return mExpression; }

		private:
			Expression* mExpression;
	};
}

#endif