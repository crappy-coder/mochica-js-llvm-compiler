#ifndef MOENJIN_AST_DOTFUNCTIONCALLEXPRESSION_H
#define MOENJIN_AST_DOTFUNCTIONCALLEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ArgumentsNode.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"

namespace MoEnjin
{
	class DotFunctionCallExpression : public Expression, public ThrowableSubExpression
	{
		public:
			DotFunctionCallExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_FUNC_CALL_DOT; }

			const UTF8String& GetId() const { return mId; }
			Expression* GetBase() const { return mBase; }
			ArgumentsNode* GetArguments() const { return mArguments; }

		private:
			Expression* mBase;
			ArgumentsNode* mArguments;
			UTF8String mId;
	};
}

#endif