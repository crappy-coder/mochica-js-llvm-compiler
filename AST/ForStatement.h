#ifndef MOENJIN_AST_FORSTATEMENT_H
#define MOENJIN_AST_FORSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ForStatement : public Statement
	{
		public:
			ForStatement(mo_int32 lineNumber, Expression* initExpr, Expression* condExpr, Expression* incExpr, Statement* statment, mo_bool firstExpressionIsVarDecl);

			virtual const NodeType GetType() const { return NODE_FOR; }

			mo_bool InitializeExpressionIsVarDecl() const { return mFirstExpressionIsVarDecl; }
			Expression* GetInitializeExpression() const { return mInitializeExpr; }
			Expression* GetConditionExpression() const { return mConditionExpr; }
			Expression* GetIncrementExpression() const { return mIncrementExpr; }
			Statement* GetStatement() const { return mStatement; }

		private:
			Expression* mInitializeExpr;
			Expression* mConditionExpr;
			Expression* mIncrementExpr;
			Statement* mStatement;
			mo_bool mFirstExpressionIsVarDecl;
	};
}

#endif