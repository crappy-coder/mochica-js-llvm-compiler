#ifndef MOENJIN_AST_PREFIXBRACKETEXPRESSION_H
#define MOENJIN_AST_PREFIXBRACKETEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowablePrefixedSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class PrefixBracketExpression : public Expression, public ThrowablePrefixedSubExpression
	{
		public:
			PrefixBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_PREFIX_BRACKET; }

			Expression* GetBase() const { return mBase; }
			Expression* GetSubscript() const { return mSubscript; }
			Operator GetOperator() const { return mOp; }

		private:
			Expression* mBase;
			Expression* mSubscript;
			Operator mOp;
	};
}

#endif