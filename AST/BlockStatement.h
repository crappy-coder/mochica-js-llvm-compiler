#ifndef MOENJIN_AST_BLOCKSTATEMENT_H
#define MOENJIN_AST_BLOCKSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/StatementList.h"

namespace MoEnjin
{
	class BlockStatement : public Statement
	{
		public:
			BlockStatement(mo_int32 lineNumber, StatementList* statements = 0);

			virtual const NodeType GetType() const { return NODE_BLOCK; }

			mo_bool HasStatements() const { return (mStatements != mo_null); }
			StatementList* GetStatements() const { return mStatements; }

		private:
			StatementList* mStatements;
	};
}

#endif