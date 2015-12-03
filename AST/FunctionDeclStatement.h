#ifndef MOENJIN_AST_FUNCTIONDECLSTATEMENT_H
#define MOENJIN_AST_FUNCTIONDECLSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "Runtime/SourceCode.h"

namespace MoEnjin
{
	class FunctionNode;
	class ParameterNode;
	class FunctionDeclStatement : public Statement
	{
		public:
			FunctionDeclStatement(int lineNumber, const UTF8String& id, FunctionNode* function, const SourceCode& source, ParameterNode* parameter);

			virtual const NodeType GetType() const { return NODE_FUNCTION_DECL; }

			FunctionNode* GetFunction() const { return mFunction; }

		private:
			FunctionNode* mFunction;
	};
}

#endif