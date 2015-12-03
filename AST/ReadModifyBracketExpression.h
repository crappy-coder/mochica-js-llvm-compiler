#ifndef MOENJIN_AST_READMODIFYBRACKETEXPRESSION_H
#define MOENJIN_AST_READMODIFYBRACKETEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class ReadModifyBracketExpression : public Expression, public ThrowableSubExpression
	{
		public:
			ReadModifyBracketExpression(mo_int32 lineNumber, Expression* base, Expression* subscript, Operator op, Expression* right, mo_bool subscriptHasAssignments, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_READ_MODIFY_BRACKET; }

			Expression* GetBase() const { return mBase; }
			Expression* GetSubscript() const { return mSubscript; }
			Expression* GetRight() const { return mRight; }
			mo_bool GetSubscriptHasAssignments() const { return mSubscriptHasAssignments; }
			mo_bool GetRightHasAssignments() const { return mRightHasAssignments; }
			Operator GetOp() const { return mOp; }

		private:
			Expression* mBase;
			Expression* mSubscript;
			Expression* mRight;
			Operator mOp : 30;
			mo_bool mSubscriptHasAssignments : 1;
			mo_bool mRightHasAssignments : 1;
	};
}

#endif