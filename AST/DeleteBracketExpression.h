#ifndef MOENJIN_AST_DELETEBRACKETEXPRESSION_H
#define MOENJIN_AST_DELETEBRACKETEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class DeleteBracketExpression : public Expression, public ThrowableExpression
	{
		public:
			DeleteBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_DELETE_BRACKET; }

			Expression* GetBase() const { return mBase; }
			Expression* GetSubscript() const { return mSubscript; }

		private:
			Expression* mBase;
			Expression* mSubscript;
	};
}

#endif