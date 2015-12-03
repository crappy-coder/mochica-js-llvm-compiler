#ifndef MOENJIN_AST_POSTFIXDOTEXPRESSION_H
#define MOENJIN_AST_POSTFIXDOTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class PostfixDotExpression : public Expression, public ThrowableSubExpression
	{
		public:
			PostfixDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_POSTFIX_DOT; }

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