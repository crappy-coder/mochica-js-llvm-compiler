#ifndef MOENJIN_AST_FUNCTIONEXPRESSION_H
#define MOENJIN_AST_FUNCTIONEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "Runtime/SourceCode.h"

namespace MoEnjin
{
	class FunctionNode;
	class ParameterNode;
	class FunctionExpression : public Expression
	{
		public:
			FunctionExpression(mo_int32 lineNumber, const UTF8String& id, FunctionNode* function, const SourceCode& source, ParameterNode* parameter);

			virtual const NodeType GetType() const { return NODE_FUNCTION_EXPR; }

			FunctionNode* GetFunction() const { return mFunction; }

		private:
			FunctionNode* mFunction;
	};
}

#endif