#ifndef MOENJIN_AST_PREFIXRESOLVEEXPRESSION_H
#define MOENJIN_AST_PREFIXRESOLVEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/PrePostResolveExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class PrefixResolveExpression : public PrePostResolveExpression
	{
		public:
			PrefixResolveExpression(mo_int32 lineNumber, Operator op, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_PREFIX_RESOLVE; }

			Operator GetOperator() const { return mOp; }

		private:
			Operator mOp;
	};
}

#endif