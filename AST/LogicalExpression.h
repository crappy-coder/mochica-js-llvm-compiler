#ifndef MOENJIN_AST_LOGICALEXPRESSION_H
#define MOENJIN_AST_LOGICALEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class LogicalExpression : public Expression
	{
		public:
			LogicalExpression(mo_int32 lineNumber, Expression* left, Expression* right, LogicalOperator op);

			virtual const NodeType GetType() const { return NODE_LOGICAL_OP; }

			Expression* GetLeft() const { return mLeft; }
			Expression* GetRight() const { return mRight; }
			LogicalOperator GetOp() const { return mOp; }

		private:
			Expression* mLeft;
			Expression* mRight;
			LogicalOperator mOp;
	};
}

#endif