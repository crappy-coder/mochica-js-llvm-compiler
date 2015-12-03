#ifndef MOENJIN_AST_PREFIXERROREXPRESSION_H
#define MOENJIN_AST_PREFIXERROREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class PrefixErrorExpression : public Expression, public ThrowableSubExpression
	{
		public:
			PrefixErrorExpression(mo_int32 lineNumber, Expression* expression, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_PREFIX_ERROR; }

			Expression* GetExpression() const { return mExpression; }
			Operator GetOperator() const { return mOp; }

		private:
			Expression* mExpression;
			Operator mOp;
	};
}

#endif