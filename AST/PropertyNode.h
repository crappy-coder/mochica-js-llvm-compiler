#ifndef MOENJIN_AST_PROPERTYNODE_H
#define MOENJIN_AST_PROPERTYNODE_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/PropertyType.h"

namespace MoEnjin
{
	class PropertyNode
	{
		friend class PropertyListNode;

		public:
			PropertyNode(const UTF8String& name, Expression* assign, PropertyType type);
			PropertyNode(mo_double name, Expression* assign, PropertyType type);

			const UTF8String& GetName() const { return mName; }
			PropertyType GetType() const { return mType; }

		private:
			UTF8String mName;
			Expression* mAssign;
			PropertyType mType;
	};
}

#endif