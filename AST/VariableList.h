#ifndef MOENJIN_AST_VARIABLELIST_H
#define MOENJIN_AST_VARIABLELIST_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class VariableList
	{
		typedef std::vector<mo_unsigned> AttributeCollection;

		public:
			VariableList() { }

			const UTF8String& GetId(mo_int32 index) const { return mNames.at(index); }
			const UTF8StringCollection& GetNames() const { return mNames; }
			const mo_unsigned GetAttributes(mo_int32 index) const { return mAttributes.at(index); }
			const mo_uint32 GetLength() const { return mNames.size(); }

			void Append(const UTF8String& id, mo_unsigned attrs)
			{
				mNames.push_back(id);
				mAttributes.push_back(attrs);
			}

		private:
			UTF8StringCollection mNames;
			AttributeCollection mAttributes;
	};
}

#endif