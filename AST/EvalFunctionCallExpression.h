#ifndef MOENJIN_AST_EVALFUNCTIONCALLEXPRESSION_H
#define MOENJIN_AST_EVALFUNCTIONCALLEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ArgumentsNode.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class EvalFunctionCallExpression : public Expression, public ThrowableExpression
	{
		public:
			EvalFunctionCallExpression(mo_int32 lineNumber, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_FUNC_CALL_EVAL; }

			ArgumentsNode* GetArguments() const { return mArgs; }

		private:
			ArgumentsNode* mArgs;
	};
}

#endif