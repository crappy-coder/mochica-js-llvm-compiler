#include "AST/ArgumentsNode.h"

namespace MoEnjin
{
	ArgumentsNode::ArgumentsNode()
		: mList(0) { }

	ArgumentsNode::ArgumentsNode(ArgumentListNode* list)
		: mList(list) { }
}