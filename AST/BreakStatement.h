#ifndef MOENJIN_AST_BREAKSTATEMENT_H
#define MOENJIN_AST_BREAKSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class BreakStatement : public Statement, public ThrowableExpression
	{
		public:
			BreakStatement(int lineNumber);
			BreakStatement(int lineNumber, const UTF8String& id);
			
			virtual const NodeType GetType() const { return NODE_BREAK; }

			const UTF8String& GetId() const { return mId; }

		private:
			UTF8String mId;
	};
}

#endif