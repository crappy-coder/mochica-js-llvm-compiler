#ifndef MOENJIN_AST_CONSTSTATEMENT_H
#define MOENJIN_AST_CONSTSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/ConstDeclExpression.h"

namespace MoEnjin
{
	class ConstStatement : public Statement
	{
		public:
			ConstStatement(mo_int32 lineNumber, ConstDeclExpression* expr);

			virtual const NodeType GetType() const { return NODE_CONST_STATEMENT; }

			ConstDeclExpression* GetExpression() const { return mExpression; }

		private:
			ConstDeclExpression* mExpression;
	};
}

#endif