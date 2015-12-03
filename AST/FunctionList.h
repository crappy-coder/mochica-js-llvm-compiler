#ifndef MOENJIN_AST_FUNCTIONLIST_H
#define MOENJIN_AST_FUNCTIONLIST_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class FunctionNode;
	class FunctionList
	{
		typedef std::vector<FunctionNode*> FunctionCollection;

		public:
			FunctionList() { }

			mo_bool HasFunctions() const { return !mItems.empty(); }
			mo_uint32 GetLength() const { return mItems.size(); }
			FunctionNode* GetFunction(const mo_int32 index) const { return mItems.at(index); }

			void SetParent(FunctionNode* parent);
			void Append(FunctionNode* function) { mItems.push_back(function); }

		private:
			FunctionCollection mItems;
	};
}

#endif