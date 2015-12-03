#include "AST/ArgumentListNode.h"

namespace MoEnjin
{
	ArgumentListNode::ArgumentListNode(mo_int32 lineNumber, Expression* expression)
		: Node(lineNumber)
		, mExpression(expression)
		, mNext(0) { }

	ArgumentListNode::ArgumentListNode(mo_int32 lineNumber, ArgumentListNode* next, Expression* expression)
		: Node(lineNumber)
		, mExpression(expression)
		, mNext(0)
	{
		next->mNext = this;
	}
}