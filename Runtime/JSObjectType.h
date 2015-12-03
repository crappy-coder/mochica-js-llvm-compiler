#ifndef MOENJIN_RUNTIME_JSOBJECTTYPE_H
#define MOENJIN_RUNTIME_JSOBJECTTYPE_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	enum JSObjectType
	{
		JSObjectTypeUnknown = 0,
		JSObjectTypeString,
		JSObjectTypeGetterSetter,

		// object types should be added after JSObjectTypeObject, otherwise IsObject will
		// reject it as being a valid js object
		JSObjectTypeObject,
		JSObjectTypeGlobal,
		JSObjectTypeEnvironment,
		JSObjectTypeFunction,
		JSObjectTypeError,
		JSObjectTypeObjectNumber,
		JSObjectTypeObjectString,
		JSObjectTypeObjectBoolean,
		JSObjectTypeObjectArray,
		JSObjectTypeObjectDate,
		JSObjectTypeObjectRegExp
	};
}

#endif