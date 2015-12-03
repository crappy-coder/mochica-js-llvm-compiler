#ifndef MOENJIN_AST_READMODIFYDOTEXPRESSION_H
#define MOENJIN_AST_READMODIFYDOTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableSubExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class ReadModifyDotExpression : public Expression, public ThrowableSubExpression
	{
		public:
			ReadModifyDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, Operator op, Expression* right, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_READ_MODIFY_DOT; }

			const UTF8String& GetId() const { return mId; }
			Expression* GetBase() const { return mBase; }
			Expression* GetRight() const { return mRight; }
			mo_bool GetRightHasAssignments() const { return mRightHasAssignments; }
			Operator GetOp() const { return mOp; }

		private:
			UTF8String mId;
			Operator mOp : 31;
			Expression* mBase;
			Expression* mRight;
			mo_bool mRightHasAssignments : 1;
	};
}

#endif