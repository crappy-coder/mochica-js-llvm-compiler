#ifndef MOENJIN_RUNTIME_TABLES_JSGLOBALOBJECTTABLE_H
#define MOENJIN_RUNTIME_TABLES_JSGLOBALOBJECTTABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSGlobalObjectTableValues[12] = {
		{ "parseInt",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_ParseInt, 2) },
		{ "parseFloat",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_ParseFloat, 1) },
		{ "isNaN",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_IsNaN, 1) },
		{ "isFinite",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_IsFinite, 1) },
		{ "escape",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_Escape, 1) },
		{ "unescape",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_Unescape, 1) },
		{ "decodeURI",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_DecodeURI, 1) },
		{ "decodeURIComponent",	JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_DecodeURIComponent, 1) },
		{ "encodeURI",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_EncodeURI, 1) },
		{ "encodeURIComponent",	JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_EncodeURIComponent, 1) },
		{ "print",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSGlobalObject_Print, 1) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSGlobalObjectTable = { JSGlobalObjectTableValues, mo_false };
}

#endif