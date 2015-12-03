#ifndef MOENJIN_AST_BITWISENOTEXPRESSION_H
#define MOENJIN_AST_BITWISENOTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class BitwiseNotExpression : public Expression
	{
		public:
			BitwiseNotExpression(mo_int32 lineNumber, Expression* expression);

			const NodeType GetType() const { return NODE_BITWISE_NOT; }
			Expression* GetExpression() const { return mExpression; }

		private:
			Expression* mExpression;
	};
}

#endif