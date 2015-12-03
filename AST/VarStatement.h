#ifndef MOENJIN_AST_VARSTATEMENT_H
#define MOENJIN_AST_VARSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class VarStatement : public Statement
	{
		public:
			VarStatement(mo_int32 lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_VAR; }

			Expression* GetExpression() const { return mExpression; }

		private:
			Expression* mExpression;

	};
}

#endif