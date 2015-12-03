#ifndef MOENJIN_AST_BRACKETACCESSOREXPRESSION_H
#define MOENJIN_AST_BRACKETACCESSOREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class BracketAccessorExpression : public Expression, public ThrowableExpression
	{
		public:
			BracketAccessorExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, mo_bool subscriptHasAssignments);

			virtual const NodeType GetType() const { return NODE_BRACKET_ACCESSOR; }

			Expression* GetBase() const { return mBase; }
			Expression* GetSubscript() const { return mSubscript; }

		private:
			Expression* mBase;
			Expression* mSubscript;
			mo_bool mSubscriptHasAssignments;
	};
}

#endif