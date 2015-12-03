#ifndef MOENJIN_AST_DELETEDOTEXPRESSION_H
#define MOENJIN_AST_DELETEDOTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class DeleteDotExpression : public Expression, public ThrowableExpression
	{
		public:
			DeleteDotExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_DELETE_DOT; }

			const UTF8String& GetID() const { return mId; }
			Expression* GetBase() const { return mBase; }

		private:
			Expression* mBase;
			UTF8String mId;
	};
}

#endif