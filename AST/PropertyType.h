#ifndef MOENJIN_AST_PROPERTYTYPE_H
#define MOENJIN_AST_PROPERTYTYPE_H

namespace MoEnjin
{
	enum PropertyType
	{
		PropertyTypeUnknown = 0,
		PropertyTypeConstant = 1,
		PropertyTypeGetter = 2,
		PropertyTypeSetter = 4
	};
}

#endif