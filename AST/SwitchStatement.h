#ifndef MOENJIN_AST_SWITCHSTATEMENT_H
#define MOENJIN_AST_SWITCHSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"
#include "AST/CaseBlockNode.h"

namespace MoEnjin
{
	class SwitchStatement : public Statement
	{
		public:
			SwitchStatement(int lineNumber, Expression* expression, CaseBlockNode* block);

			virtual const NodeType GetType() const { return NODE_SWITCH; }

			Expression* GetExpression() const { return mExpression; }
			CaseBlockNode* GetBlock() const { return mBlock; }

		private:
			CaseBlockNode* mBlock;
			Expression* mExpression;
	};
}

#endif