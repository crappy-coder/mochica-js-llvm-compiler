#ifndef MOENJIN_AST_REGEXEXPRESSION_H
#define MOENJIN_AST_REGEXEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class RegExExpression : public Expression, public ThrowableExpression
	{
		public:
			RegExExpression(int lineNumber, const UTF8String& pattern, const UTF8String& flags);

			virtual const NodeType GetType() const { return NODE_REGEX; }

			const UTF8String& GetPattern() const { return mPattern; }
			const UTF8String& GetFlags() const { return mFlags; }

		private:
			UTF8String mPattern;
			UTF8String mFlags;
	};
}

#endif