#ifndef MOENJIN_AST_VOIDEXPRESSION_H
#define MOENJIN_AST_VOIDEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class VoidExpression : public Expression
	{
		public:
			VoidExpression(mo_int32 lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_VOID; }

			Expression* GetExpression() const { return mExpression; }

		private:
			Expression* mExpression;
	};
}

#endif