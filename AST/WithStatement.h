#ifndef MOENJIN_AST_WITHSTATEMENT_H
#define MOENJIN_AST_WITHSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class WithStatement : public Statement
	{
		public:
			WithStatement(mo_int32 lineNumber, Expression* expression, Statement* statement, mo_uint32 divot, mo_uint32 expressionLength);

			virtual const NodeType GetType() const { return NODE_WITH; }

		private:
			Expression* mExpression;
			Statement* mStatement;
			mo_uint32 mDivot;
			mo_uint32 mExpressionLength;
	};
}

#endif