#ifndef MOENJIN_RUNTIME_TABLES_JSNUMBERPROTOTYPETABLE_H
#define MOENJIN_RUNTIME_TABLES_JSNUMBERPROTOTYPETABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSNumberPrototypeTableValues[7] = {
		{ "toString",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSNumberPrototype_ToString, 1) },
		{ "toLocaleString", JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSNumberPrototype_ToLocaleString, 0) },
		{ "toFixed",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSNumberPrototype_ToFixed, 1) },
		{ "toExponential",	JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSNumberPrototype_ToExponential, 1) },
		{ "toPrecision",	JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSNumberPrototype_ToPrecision, 1) },
		{ "valueOf",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSNumberPrototype_ValueOf, 0) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSNumberPrototypeTable = { JSNumberPrototypeTableValues, mo_false };
}

#endif