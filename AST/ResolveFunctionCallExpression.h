#ifndef MOENJIN_AST_RESOLVEFUNCTIONCALLEXPRESSION_H
#define MOENJIN_AST_RESOLVEFUNCTIONCALLEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ArgumentsNode.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class ResolveFunctionCallExpression : public Expression, public ThrowableExpression
	{
		public:
			ResolveFunctionCallExpression(mo_int32 lineNumber, const UTF8String& id, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_FUNC_CALL_RESOLVE; }

			const UTF8String& GetId() const { return mId; }
			ArgumentsNode* GetArguments() const { return mArgs; }

		private:
			UTF8String mId;
			ArgumentsNode* mArgs;
			mo_int32 mScopeDepth;
	};
}

#endif