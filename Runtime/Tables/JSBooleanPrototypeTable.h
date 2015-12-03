#ifndef MOENJIN_RUNTIME_TABLES_JSBOOLEANPROTOTYPETABLE_H
#define MOENJIN_RUNTIME_TABLES_JSBOOLEANPROTOTYPETABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSBooleanPrototypeTableValues[3] = {
		{ "toString",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSBooleanPrototype_ToString, 0) },
		{ "valueOf",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSBooleanPrototype_ValueOf, 0) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSBooleanPrototypeTable = { JSBooleanPrototypeTableValues, mo_false };
}

#endif