#ifndef MOENJIN_AST_CLAUSELISTNODE_H
#define MOENJIN_AST_CLAUSELISTNODE_H

#include "Platform/Configuration.h"
#include "AST/CaseClauseNode.h"

namespace MoEnjin
{
	class ClauseListNode
	{
		public:
			ClauseListNode(CaseClauseNode* clause);
			ClauseListNode(CaseClauseNode* clause, ClauseListNode* next);

			CaseClauseNode* GetClause() const { return mClause; }
			ClauseListNode* GetNext() const { return mNext; }

		private:
			CaseClauseNode* mClause;
			ClauseListNode* mNext;
	};
}

#endif