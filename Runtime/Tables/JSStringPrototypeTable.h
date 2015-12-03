#ifndef MOENJIN_RUNTIME_TABLES_JSSTRINGPROTOTYPETABLE_H
#define MOENJIN_RUNTIME_TABLES_JSSTRINGPROTOTYPETABLE_H

#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	static const struct JSClassPropertyTableValue JSStringPrototypeTableValues[21] = {
		{ "toString",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_ToString, 0) },
		{ "valueOf",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_ValueOf, 0) },
		{ "charAt",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_CharAt, 1) },
		{ "charCodeAt",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_CharCodeAt, 0) },
		{ "concat",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Concat, 1) },
		{ "indexOf",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_IndexOf, 1) },
		{ "lastIndexOf",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_LastIndexOf, 1) },
		{ "localeCompare",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_LocaleCompare, 1) },
		{ "match",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Match, 1) },
		{ "replace",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Replace, 2) },
		{ "search",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Search, 1) },
		{ "slice",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Slice, 2) },
		{ "split",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Split, 2) },
		{ "substr",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Substr, 2) },
		{ "substring",			JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Substring, 2) },
		{ "toLowerCase",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_ToLowerCase, 0) },
		{ "toLocaleLowerCase",	JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_ToLocaleLowerCase, 0) },
		{ "toUpperCase",		JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_ToUpperCase, 0) },
		{ "toLocaleUpperCase",	JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_ToLocaleUpperCase, 0) },
		{ "trim",				JS_PROP_ATTR_HIDE(Function), NATIVE_FUNCTION_PTR(JSStringPrototype_Trim, 0) },
		{ 0, 0, 0, 0 }
	};

	mo_extern const struct JSClassPropertyTable JSStringPrototypeTable = { JSStringPrototypeTableValues, mo_false };
}

#endif