#ifndef MOENJIN_AST_VALUEFUNCTIONCALLEXPRESSION_H
#define MOENJIN_AST_VALUEFUNCTIONCALLEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ArgumentsNode.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class ValueFunctionCallExpression : public Expression, public ThrowableExpression
	{
		public:
			ValueFunctionCallExpression(mo_int32 lineNumber, Expression* expression, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_FUNC_CALL_VALUE; }

			Expression* GetExpression() const { return mExpression; }
			ArgumentsNode* GetArguments() const { return mArgs; }

		private:
			Expression* mExpression;
			ArgumentsNode* mArgs;
	};
}

#endif