#ifndef MOENJIN_AST_ASSIGNDOTEXPRESSION_H
#define MOENJIN_AST_ASSIGNDOTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class AssignDotExpression : public Expression, public ThrowableExpression
	{
		public:
			AssignDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, Expression* right, mo_bool rightHasAssignments, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_ASSIGN_DOT; }

			const UTF8String& GetId() const { return mId; }
			Expression* GetBase() const { return mBase; }
			Expression* GetRight() const { return mRight; }

			mo_bool RightHasAssignments() const { return mRightHasAssignments; }

		private:
			UTF8String mId;
			Expression* mBase;
			Expression* mRight;
			mo_bool mRightHasAssignments;
	};
}

#endif