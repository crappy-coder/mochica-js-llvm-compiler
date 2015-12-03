#ifndef MOENJIN_AST_DEBUGGERSTATEMENT_H
#define MOENJIN_AST_DEBUGGERSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"

namespace MoEnjin
{
	class DebuggerStatement : public Statement
	{
		public:
			DebuggerStatement(mo_int32 lineNumber);

			virtual const NodeType GetType() const { return NODE_DEBUGGER; }
	};
}

#endif