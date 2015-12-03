#ifndef MOENJIN_RUNTIME_TABLES_JSNUMBERCONSTRUCTORTABLE_H
#define MOENJIN_RUNTIME_TABLES_JSNUMBERCONSTRUCTORTABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSNumberConstructorTableValues[6] = {
		{ "NaN",				JS_PROP_ATTR_FIXED, NATIVE_GET_FUNCTION_PTR(JSNumberConstructor_GetNaN, 0) },
		{ "NEGATIVE_INFINITY",	JS_PROP_ATTR_FIXED, NATIVE_GET_FUNCTION_PTR(JSNumberConstructor_GetNegativeInfinity, 0) },
		{ "POSITIVE_INFINITY",	JS_PROP_ATTR_FIXED, NATIVE_GET_FUNCTION_PTR(JSNumberConstructor_GetPositiveInfinity, 0) },
		{ "MAX_VALUE",			JS_PROP_ATTR_FIXED, NATIVE_GET_FUNCTION_PTR(JSNumberConstructor_GetMaxValue, 0) },
		{ "MIN_VALUE",			JS_PROP_ATTR_FIXED, NATIVE_GET_FUNCTION_PTR(JSNumberConstructor_GetMinValue, 0) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSNumberConstructorTable = { JSNumberConstructorTableValues, mo_false };
}

#endif