#ifndef MOENJIN_AST_STRINGEXPRESSION_H
#define MOENJIN_AST_STRINGEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class StringExpression : public Expression
	{
		public:
			StringExpression(mo_int32 lineNumber, const UTF8String& value);

			virtual const NodeType GetType() const { return NODE_STRING; }

			const UTF8String& GetValue() { return mValue; }

		private:
			UTF8String mValue;
	};
}

#endif