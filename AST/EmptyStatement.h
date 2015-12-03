#ifndef MOENJIN_AST_EMPTYSTATEMENT_H
#define MOENJIN_AST_EMPTYSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"

namespace MoEnjin
{
	class EmptyStatement : public Statement
	{
		public:
			EmptyStatement(mo_int32 lineNumber);

			virtual const NodeType GetType() const { return NODE_EMPTY; }
	};
}

#endif