#ifndef MOENJIN_AST_ASSIGNBRACKETEXPRESSION_H
#define MOENJIN_AST_ASSIGNBRACKETEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class AssignBracketExpression : public Expression, public ThrowableExpression
	{
		public:
			AssignBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, Expression* right, mo_bool subscriptHasAssignments, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_ASSIGN_BRACKET; }

			mo_bool GetSubscriptHasAssignments() const { return mSubscriptHasAssignments; }
			mo_bool GetRightHasAssignments() const { return mRightHasAssignments; }

			Expression* GetBase() const { return mBase; }
			Expression* GetSubscript() const { return mSubscript; }
			Expression* GetRight() const { return mRight; }

		private:
			Expression* mBase;
			Expression* mSubscript;
			Expression* mRight;
			mo_bool mSubscriptHasAssignments : 1;
			mo_bool mRightHasAssignments : 1;
	};
}

#endif