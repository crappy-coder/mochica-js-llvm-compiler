#ifndef MOENJIN_AST_BRACKETFUNCTIONCALLEXPRESSION_H
#define MOENJIN_AST_BRACKETFUNCTIONCALLEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ArgumentsNode.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"

namespace MoEnjin
{
	class BracketFunctionCallExpression : public Expression, public ThrowableSubExpression
	{
		public:
			BracketFunctionCallExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_FUNC_CALL_BRACKET; }

			Expression* GetBase() const { return mBase; }
			Expression* GetSubscript() const { return mSubscript; }
			ArgumentsNode* GetArguments() const { return mArguments; }

		private:
			Expression* mBase;
			Expression* mSubscript;
			ArgumentsNode* mArguments;
	};
}

#endif