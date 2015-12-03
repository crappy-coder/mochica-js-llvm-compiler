#ifndef MOENJIN_AST_LABELSTATEMENT_H
#define MOENJIN_AST_LABELSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class LabelStatement : public Statement, public ThrowableExpression
	{
		public:
			LabelStatement(mo_int32 lineNumber, const UTF8String& name, Statement* statement);

			virtual const NodeType GetType() const { return NODE_LABEL; }

		private:
			UTF8String mName;
			Statement* mStatement;
	};
}

#endif