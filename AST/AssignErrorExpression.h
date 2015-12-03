#ifndef MOENJIN_AST_ASSIGNERROREXPRESSION_H
#define MOENJIN_AST_ASSIGNERROREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class AssignErrorExpression : public Expression, public ThrowableExpression
	{
		public:
			AssignErrorExpression(mo_int32 lineNumber, Expression* left, Expression* right, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_ASSIGN_ERROR; }

		private:
			Expression* mLeft;
			Expression* mRight;
			Operator mOp;
	};
}

#endif