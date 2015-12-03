#ifndef MOENJIN_AST_PROPERTYLISTNODE_H
#define MOENJIN_AST_PROPERTYLISTNODE_H

#include "Platform/Configuration.h"
#include "AST/Node.h"

namespace MoEnjin
{
	class PropertyNode;
	class PropertyListNode : public Node
	{
		public:
			PropertyListNode(int lineNumber, PropertyNode* prop);
			PropertyListNode(int lineNumber, PropertyNode* prop, PropertyListNode* next);

			virtual const NodeType GetType() const { return NODE_PROPERTY_LIST; }

		private:
			PropertyNode* mNode;
			PropertyListNode* mNext;
	};
}

#endif