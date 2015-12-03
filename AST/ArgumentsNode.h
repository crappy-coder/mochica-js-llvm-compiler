#ifndef MOENJIN_AST_ARGUMENTSNODE_H
#define MOENJIN_AST_ARGUMENTSNODE_H

#include "Platform/Configuration.h"
#include "AST/ArgumentListNode.h"

namespace MoEnjin
{
	class ArgumentsNode
	{
		public:
			ArgumentsNode();
			ArgumentsNode(ArgumentListNode* list);

			const mo_bool HasArguments() const { return (mList != mo_null); }
			ArgumentListNode* GetFirstArgument() const { return mList; }

		private:
			ArgumentListNode* mList;
	};
}

#endif