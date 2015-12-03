#ifndef MOENJIN_AST_CASECLAUSENODE_H
#define MOENJIN_AST_CASECLAUSENODE_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/StatementList.h"

namespace MoEnjin
{
	class CaseClauseNode
	{
		public:
			CaseClauseNode(Expression* expression, StatementList* statements);

			Expression* GetExpression() const { return mExpression; }
			StatementList* GetStatements() const { return mStatements; }

		private:
			Expression* mExpression;
			StatementList* mStatements;
	};
}

#endif