#ifndef MOENJIN_RUNTIME_TABLES_JSOBJECTCONSTRUCTORTABLE_H
#define MOENJIN_RUNTIME_TABLES_JSOBJECTCONSTRUCTORTABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSObjectConstructorTableValues[2] = {
		{ "create",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSObjectConstructor_Create, 2) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSObjectConstructorTable = { JSObjectConstructorTableValues, mo_false };
}

#endif