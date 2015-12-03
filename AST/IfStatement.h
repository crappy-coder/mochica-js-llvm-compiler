#ifndef MOENJIN_AST_IFSTATEMENT_H
#define MOENJIN_AST_IFSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class IfStatement : public Statement
	{
		public:
			IfStatement(mo_int32 lineNumber, Expression* condition, Statement* block);

			virtual const NodeType GetType() const { return NODE_IF; }

			Expression* GetCondition() const { return mCondition; }
			Statement* GetBlock() const { return mBlock; }

		private:
			Expression* mCondition;
			Statement* mBlock;
	};
}

#endif