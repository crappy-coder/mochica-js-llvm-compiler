#ifndef MOENJIN_AST_DOTACCESSOREXPRESSION_H
#define MOENJIN_AST_DOTACCESSOREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class DotAccessorExpression : public Expression, public ThrowableExpression
	{
		public:
			DotAccessorExpression(mo_int32 lineNumber, Expression* base, const UTF8String& id);

			virtual const NodeType GetType() const { return NODE_DOT_ACCESSOR; }

			Expression* GetBase() const { return mBase; }
			const UTF8String& GetID() const { return mId; }

		private:
			Expression* mBase;
			UTF8String mId;
	};
}

#endif