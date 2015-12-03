#ifndef MOENJIN_AST_CASEBLOCKNODE_H
#define MOENJIN_AST_CASEBLOCKNODE_H

#include "Platform/Configuration.h"
#include "AST/ClauseListNode.h"
#include "AST/CaseClauseNode.h"

namespace MoEnjin
{
	class CaseBlockNode
	{
		public:
			CaseBlockNode(ClauseListNode* list1, CaseClauseNode* defaultClause, ClauseListNode* list2);

			ClauseListNode* GetFirstList() const { return mList1; }
			ClauseListNode* GetSecondList() const { return mList2; }
			CaseClauseNode* GetDefaultClause() const { return mDefaultClause; }

		private:
			ClauseListNode* mList1;
			ClauseListNode* mList2;
			CaseClauseNode* mDefaultClause;
	};
}

#endif