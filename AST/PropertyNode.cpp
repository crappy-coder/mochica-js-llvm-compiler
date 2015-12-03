#include "AST/PropertyNode.h"

namespace MoEnjin
{
	PropertyNode::PropertyNode(const UTF8String& name, Expression* assign, PropertyType type)
		: mName(name)
		, mAssign(assign)
		, mType(type) { }

	PropertyNode::PropertyNode(double name, Expression* assign, PropertyType type)
		: mName(UTF8String::FromNumber(name))
		, mAssign(assign)
		, mType(type) { }
}