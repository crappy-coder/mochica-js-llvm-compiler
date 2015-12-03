#ifndef MOENJIN_RUNTIME_TABLES_JSOBJECTPROTOTYPETABLE_H
#define MOENJIN_RUNTIME_TABLES_JSOBJECTPROTOTYPETABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSObjectPrototypeTableValues[7] = {
		{ "toString",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSObjectPrototype_ToString, 0) },
		{ "toLocaleString",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSObjectPrototype_ToLocaleString, 0) },
		{ "valueOf",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSObjectPrototype_ValueOf, 0) },
		{ "hasOwnProperty",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSObjectPrototype_HasOwnProperty, 1) },
		{ "isPrototypeOf",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSObjectPrototype_IsPrototypeOf, 1) },
		{ "propertyIsEnumerable",	JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSObjectPrototype_PropertyIsEnumerable, 1) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSObjectPrototypeTable = { JSObjectPrototypeTableValues, mo_false };
}

#endif