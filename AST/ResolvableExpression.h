#ifndef MOENJIN_AST_RESOLVABLEEXPRESSION_H
#define MOENJIN_AST_RESOLVABLEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ResolvableExpression : public Expression
	{
		public:
			ResolvableExpression(mo_uint32 lineNumber, const UTF8String& id, mo_uint32 startPosition)
				: Expression(lineNumber)
				, mId(id), mStartPosition(startPosition) { }

			virtual const NodeType GetType() const { return NODE_RESOLVE; }

			const UTF8String& GetID() const { return mId; }

		private:
			UTF8String mId;
			mo_uint32 mStartPosition;
	};
}

#endif