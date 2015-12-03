#include "AST/FunctionList.h"
#include "AST/FunctionNode.h"

namespace MoEnjin
{
	void FunctionList::SetParent(FunctionNode* parent)
	{
		if(mItems.empty())
			return;

		for(FunctionCollection::iterator i = mItems.begin(); i != mItems.end(); ++i)
			(*i)->SetParent(parent);
	}
}