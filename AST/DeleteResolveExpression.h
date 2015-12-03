#ifndef MOENJIN_AST_DELETERESOLVEEXPRESSION_H
#define MOENJIN_AST_DELETERESOLVEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class DeleteResolveExpression : public Expression, public ThrowableExpression
	{
		public:
			DeleteResolveExpression(mo_int32 lineNumber, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);
			
			virtual const NodeType GetType() const { return NODE_DELETE_RESOLVE; }

			const UTF8String& GetID() const { return mId; }

		private:
			UTF8String mId;
	};
}

#endif