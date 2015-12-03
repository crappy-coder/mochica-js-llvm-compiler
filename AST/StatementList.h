#ifndef MOENJIN_AST_STATEMENTLIST_H
#define MOENJIN_AST_STATEMENTLIST_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class Statement;
	class StatementList
	{
		typedef std::vector<Statement*> StatementCollection;

		public:
			StatementList() { }

			mo_bool HasStatements() const { return !mItems.empty(); }
			mo_uint32 GetLength() const { return mItems.size(); }
			Statement* GetStatement(const mo_int32 index) const { return mItems.at(index); }

			void Append(Statement* statement) { mItems.push_back(statement); }

		private:
			StatementCollection mItems;
	};
}

#endif