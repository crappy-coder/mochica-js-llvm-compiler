#ifndef MOENJIN_AST_CONTINUESTATEMENT_H
#define MOENJIN_AST_CONTINUESTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/ThrowableExpression.h"
#include "Runtime/GlobalContext.h"

namespace MoEnjin
{
	class ContinueStatement : public Statement, public ThrowableExpression
	{
		public:
			ContinueStatement(GlobalContext* globalContext, int lineNumber);
			ContinueStatement(int lineNumber, const UTF8String& id);
			
			virtual const NodeType GetType() const { return NODE_CONTINUE; }

			const UTF8String& GetId() const { return mId; }

		private:
			UTF8String mId;
	};
}

#endif