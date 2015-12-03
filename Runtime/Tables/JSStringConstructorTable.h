#ifndef MOENJIN_RUNTIME_TABLES_JSSTRINGCONSTRUCTORTABLE_H
#define MOENJIN_RUNTIME_TABLES_JSSTRINGCONSTRUCTORTABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSStringConstructorTableValues[2] = {
		{ "fromCharCode", JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringConstructor_FromCharCode, 1) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSStringConstructorTable = { JSStringConstructorTableValues, mo_false };
}

#endif