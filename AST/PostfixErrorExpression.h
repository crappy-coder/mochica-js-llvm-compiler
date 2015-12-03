#ifndef MOENJIN_AST_POSTFIXERROREXPRESSION_H
#define MOENJIN_AST_POSTFIXERROREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class PostfixErrorExpression : public Expression, public ThrowableSubExpression
	{
		public:
			PostfixErrorExpression(mo_int32 lineNumber, Expression* expression, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_POSTFIX_ERROR; }

			Expression* GetExpression() const { return mExpression; }
			Operator GetOperator() const { return mOp; }

		private:
			Expression* mExpression;
			Operator mOp;
	};
}

#endif