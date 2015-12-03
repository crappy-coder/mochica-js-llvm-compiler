#ifndef MOENJIN_AST_ASSIGNRESOLVEEXPRESSION_H
#define MOENJIN_AST_ASSIGNRESOLVEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"
#include "AST/Operators.h"

namespace MoEnjin
{
	class AssignResolveExpression : public Expression, public ThrowableExpression
	{
		public:
			AssignResolveExpression(mo_int32 lineNumber, const UTF8String& id, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_ASSIGN_RESOLVE; }

			const UTF8String& GetId() const { return mId; }
			Expression* GetRight() const { return mRight; }
			const mo_bool GetRightHasAssignments() const { return mRightHasAssignments; }

		private:
			UTF8String mId;
			Expression* mRight;
			mo_bool mRightHasAssignments;
	};
}

#endif