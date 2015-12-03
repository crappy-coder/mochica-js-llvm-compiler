#ifndef MOENJIN_AST_WHILESTATEMENT_H
#define MOENJIN_AST_WHILESTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class WhileStatement : public Statement
	{
		public:
			WhileStatement(mo_int32 lineNumber, Expression* expression, Statement* statement);

			virtual const NodeType GetType() const { return NODE_WHILE; }

			Expression* GetExpression() const { return mExpression; }
			Statement* GetStatement() const { return mStatement; }
			
		private:
			Expression* mExpression;
			Statement* mStatement;
	};
}

#endif