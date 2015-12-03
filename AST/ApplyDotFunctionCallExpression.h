#ifndef MOENJIN_AST_APPLYDOTFUNCTIONCALLEXPRESSION_H
#define MOENJIN_AST_APPLYDOTFUNCTIONCALLEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/DotFunctionCallExpression.h"

namespace MoEnjin
{
	class ApplyDotFunctionCallExpression : public DotFunctionCallExpression
	{
		public:
			ApplyDotFunctionCallExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, ArgumentsNode* args, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_FUNCTION_APPLY; }
	};
}

#endif