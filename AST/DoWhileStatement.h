#ifndef MOENJIN_AST_DOWHILESTATEMENT_H
#define MOENJIN_AST_DOWHILESTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class DoWhileStatement : public Statement
	{
		public:
			DoWhileStatement(mo_int32 lineNumber, Statement* statement, Expression* expression);

			virtual const NodeType GetType() const { return NODE_DO_WHILE; }

			Expression* GetExpression() const { return mExpression; }
			Statement* GetStatement() const { return mStatement; }

		private:
			Statement* mStatement;
			Expression* mExpression;
	};
}

#endif