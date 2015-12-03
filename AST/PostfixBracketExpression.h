#ifndef MOENJIN_AST_POSTFIXBRACKETEXPRESSION_H
#define MOENJIN_AST_POSTFIXBRACKETEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class PostfixBracketExpression : public Expression, public ThrowableSubExpression
	{
		public:
			PostfixBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, Operator op, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_POSTFIX_BRACKET; }

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