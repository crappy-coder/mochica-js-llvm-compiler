#include "AST/ParameterNode.h"

namespace MoEnjin
{
	ParameterNode::ParameterNode(const UTF8String& id)
		: mId(id), mNext(0) { }

	ParameterNode::ParameterNode(const UTF8String& id, ParameterNode* next)
		: mId(id), mNext(0)
	{
		next->mNext = this;
	}
}