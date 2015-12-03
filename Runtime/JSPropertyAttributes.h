#ifndef MOENJIN_RUNTIME_JSPROPERTYATTRIBUTES_H
#define MOENJIN_RUNTIME_JSPROPERTYATTRIBUTES_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	enum JSPropertyAttributes
	{
		JSPropertyAttributeNone = 0,
		JSPropertyAttributeReadOnly = 1 << 1,
		JSPropertyAttributeDontEnum = 1 << 2,
		JSPropertyAttributeDontDelete = 1 << 3,
		JSPropertyAttributeFunction = 1 << 4,
		JSPropertyAttributeAccessor = 1 << 5
	};

	#define JS_PROP_ATTR(attr)		(JSPropertyAttribute##attr)
	#define JS_PROP_ATTR_HIDE(attr)	(JSPropertyAttributeDontEnum|JSPropertyAttribute##attr)
	#define JS_PROP_ATTR_FIXED		(JSPropertyAttributeDontEnum|JSPropertyAttributeDontDelete|JSPropertyAttributeReadOnly)
}

#endif