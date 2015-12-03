#ifndef MOENJIN_AST_IFELSESTATEMENT_H
#define MOENJIN_AST_IFELSESTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/IfStatement.h"

namespace MoEnjin
{
	class IfElseStatement : public IfStatement
	{
		public:
			IfElseStatement(mo_int32 lineNumber, Expression* condition, Statement* ifBlock, Statement* elseBlock);

			virtual const NodeType GetType() const { return NODE_IF_ELSE; }

			Statement* GetElseBlock() const { return mElseBlock; }

		private:
			Statement* mElseBlock;
	};
}

#endif