#include "AST/PropertyListNode.h"
#include "AST/PropertyNode.h"

namespace MoEnjin
{
	PropertyListNode::PropertyListNode(int lineNumber, PropertyNode* prop)
		: Node(lineNumber)
		, mNode(prop)
		, mNext(0) { }

	PropertyListNode::PropertyListNode(int lineNumber, PropertyNode* prop, PropertyListNode* next)
		: Node(lineNumber)
		, mNode(prop)
		, mNext(0)
	{
		next->mNext = this;
	}
}