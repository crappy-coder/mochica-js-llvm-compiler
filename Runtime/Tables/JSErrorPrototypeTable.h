#ifndef MOENJIN_RUNTIME_TABLES_JSERRORPROTOTYPETABLE_H
#define MOENJIN_RUNTIME_TABLES_JSERRORPROTOTYPETABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSErrorPrototypeTableValues[2] = {
		{ "toString",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSErrorPrototype_ToString, 0) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSErrorPrototypeTable = { JSErrorPrototypeTableValues, mo_false };
}

#endif