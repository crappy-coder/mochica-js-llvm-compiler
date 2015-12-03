#ifndef MOENJIN_AST_RETURNSTATEMENT_H
#define MOENJIN_AST_RETURNSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/ThrowableExpression.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ReturnStatement : public Statement, public ThrowableExpression
	{
		public:
			ReturnStatement(mo_int32 lineNumber, Expression* value);

			virtual const NodeType GetType() const { return NODE_RETURN; }

			Expression* GetValue() const { return mValue; }

		private:
			Expression* mValue;
	};
}

#endif