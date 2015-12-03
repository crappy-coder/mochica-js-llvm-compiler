#ifndef MOENJIN_AST_TYPEOFRESOLVEEXPRESSION_H
#define MOENJIN_AST_TYPEOFRESOLVEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class TypeOfResolveExpression : public Expression
	{
		public:
			TypeOfResolveExpression(mo_int32 lineNumber, const UTF8String& id);

			virtual const NodeType GetType() const { return NODE_TYPEOF_RESOLVE; }

			const UTF8String& GetId() const { return mId; }

		private:
			UTF8String mId;
	};
}

#endif