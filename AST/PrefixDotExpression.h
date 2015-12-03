#ifndef MOENJIN_AST_PREFIXDOTEXPRESSION_H
#define MOENJIN_AST_PREFIXDOTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowablePrefixedSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class PrefixDotExpression : public Expression, public ThrowablePrefixedSubExpression
	{
		public:
			PrefixDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_PREFIX_DOT; }

			const UTF8String& GetId() const { return mId; }
			Expression* GetBase() const { return mBase; }
			Operator GetOperator() const { return mOp; }

		private:
			Expression* mBase;
			Operator mOp;
			UTF8String mId;
	};
}

#endif