#include "AST/CaseBlockNode.h"

namespace MoEnjin
{
	CaseBlockNode::CaseBlockNode(ClauseListNode* list1, CaseClauseNode* defaultClause, ClauseListNode* list2)
		: mList1(list1)
		, mList2(list2)
		, mDefaultClause(defaultClause) { }
}