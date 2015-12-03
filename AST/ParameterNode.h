#ifndef MOENJIN_AST_PARAMETERNODE_H
#define MOENJIN_AST_PARAMETERNODE_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class ParameterNode
	{
		public:
			ParameterNode(const UTF8String& id);
			ParameterNode(const UTF8String& id, ParameterNode* next);
			
			const UTF8String& GetId() const { return mId; }
			ParameterNode* GetNext() const { return mNext; }

		private:
			UTF8String mId;
			ParameterNode* mNext;
	};
}

#endif