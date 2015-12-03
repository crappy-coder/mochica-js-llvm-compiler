#ifndef MOENJIN_AST_READMODIFYRESOLVEEXPRESSION_H
#define MOENJIN_AST_READMODIFYRESOLVEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class ReadModifyResolveExpression : public Expression, public ThrowableExpression
	{
		public:
			ReadModifyResolveExpression(mo_int32 lineNumber, const UTF8String& id, Operator op, Expression* right, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_READ_MODIFY_RESOLVE; }

			const UTF8String& GetId() const { return mId; }
			Expression* GetRight() const { return mRight; }
			mo_bool GetRightHasAssignment() const { return mRightHasAssignments; }
			Operator GetOp() const { return mOp; }

		private:
			UTF8String mId;
			Expression* mRight;
			mo_bool mRightHasAssignments;
			Operator mOp;
	};
}

#endif