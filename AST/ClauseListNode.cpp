#include "AST/ClauseListNode.h"

namespace MoEnjin
{
	ClauseListNode::ClauseListNode(CaseClauseNode* clause)
		: mClause(clause)
		, mNext(0) { }

	ClauseListNode::ClauseListNode(CaseClauseNode* clause, ClauseListNode* next)
		: mClause(clause)
		, mNext(0)
	{
		next->mNext = this;
	}
}