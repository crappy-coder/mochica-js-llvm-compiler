#ifndef MOENJIN_RUNTIME_JSCLASSFUNCTIONTABLE_H
#define MOENJIN_RUNTIME_JSCLASSFUNCTIONTABLE_H

#include "Platform/Configuration.h"
#include "Runtime/JSPropertyDescriptor.h"
#include "Runtime/JSPropertyNameList.h"
#include "Runtime/JSPropertySlot.h"
#include "Runtime/JSPutPropertySlot.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSObject;
	class JSStackFrame;
	struct JSCallInfo;
	struct JSClassFunctionTable
	{
		typedef UTF8String (*GetClassNameFunctionPtr)(const JSObject*);
		GetClassNameFunctionPtr GetClassNameImpl;

		typedef mo_bool (*IsExtensibleFunctionPtr)(const JSObject*);
		IsExtensibleFunctionPtr IsExtensibleImpl;

		typedef mo_bool (*IsCallableFunctionPtr)();
		IsCallableFunctionPtr IsCallableImpl;

		typedef mo_bool (*ImplementsHasInstanceFunctionPtr)();
		ImplementsHasInstanceFunctionPtr ImplementsHasInstanceImpl;

		typedef void (*GetOwnPropertyNamesFunctionPtr)(JSStackFrame*, JSObject*, JSPropertyNameList&, mo_bool);
		GetOwnPropertyNamesFunctionPtr GetOwnPropertyNamesImpl;

		typedef void (*GetPropertyNamesFunctionPtr)(JSStackFrame*, JSObject*, JSPropertyNameList&, mo_bool);
		GetPropertyNamesFunctionPtr GetPropertyNamesImpl;

		typedef mo_bool (*GetOwnPropertyFunctionPtr)(JSStackFrame*, JSObject*, const UTF8String&, JSPropertyDescriptor&);
		GetOwnPropertyFunctionPtr GetOwnPropertyDescriptorImpl;

		typedef mo_bool (*GetOwnPropertySlotFunctionPtr)(JSStackFrame*, JSObject*, const UTF8String&, JSPropertySlot&);
		GetOwnPropertySlotFunctionPtr GetOwnPropertySlotImpl;

		typedef mo_bool (*GetOwnPropertySlotByIndexFunctionPtr)(JSStackFrame*, JSObject*, mo_unsigned, JSPropertySlot&);
		GetOwnPropertySlotByIndexFunctionPtr GetOwnPropertySlotByIndexImpl;

		typedef void (*PutFunctionPtr)(JSStackFrame*, JSObject*, const UTF8String&, JSValue, mo_unsigned attributes);
		PutFunctionPtr PutImpl;

		typedef void (*PutPropertyFunctionPtr)(JSStackFrame*, JSObject*, const UTF8String&, JSValue, mo_bool, JSPutPropertySlot&);
		PutPropertyFunctionPtr PutPropertyImpl;

		typedef void (*PutPropertyByIndexFunctionPtr)(JSStackFrame*, JSObject*, mo_unsigned, JSValue, mo_bool);
		PutPropertyByIndexFunctionPtr PutPropertyByIndexImpl;

		typedef mo_bool (*DeletePropertyFunctionPtr)(JSStackFrame*, JSObject*, const UTF8String&, mo_bool, mo_bool);
		DeletePropertyFunctionPtr DeletePropertyImpl;

		typedef mo_bool (*DeletePropertyByIndexFunctionPtr)(JSStackFrame*, JSObject*, mo_unsigned, mo_bool, mo_bool);
		DeletePropertyByIndexFunctionPtr DeletePropertyByIndexImpl;

		typedef mo_bool (*DefineOwnPropertyFunctionPtr)(JSStackFrame*, JSObject*, const UTF8String&, JSPropertyDescriptor&, mo_bool);
		DefineOwnPropertyFunctionPtr DefineOwnPropertyImpl;

		typedef JSValue (*GetDefaultValueFunctionPtr)(JSStackFrame*, const JSObject*, JSTypePreference);
		GetDefaultValueFunctionPtr GetDefaultValueImpl;

		typedef JSObject* (*ToThisObjectFunctionPtr)(JSStackFrame*, JSObject*);
		ToThisObjectFunctionPtr ToThisObjectImpl;

		typedef mo_bool (*HasInstanceFunctionPtr)(JSStackFrame*, JSObject*, JSValue, JSValue);
		HasInstanceFunctionPtr HasInstanceImpl;

		typedef JSCallInfo (*GetCallInfoFunctionPtr)(JSStackFrame*, JSObject*);
		GetCallInfoFunctionPtr GetCallInfoImpl;

		typedef JSCallInfo (*GetConstructInfoFunctionPtr)(JSStackFrame*, JSObject*);
		GetConstructInfoFunctionPtr GetConstructInfoImpl;
	};

#define CREATE_CLASS_FUNCTION_TABLE(cls)	{	\
		&cls::GetClassNameImpl,					\
		&cls::IsExtensibleImpl,					\
		&cls::IsCallableImpl,					\
		&cls::ImplementsHasInstanceImpl,		\
		&cls::GetOwnPropertyNamesImpl,			\
		&cls::GetPropertyNamesImpl,				\
		&cls::GetOwnPropertyDescriptorImpl,		\
		&cls::GetOwnPropertySlotImpl,			\
		&cls::GetOwnPropertySlotByIndexImpl,	\
		&cls::PutImpl,							\
		&cls::PutPropertyImpl,					\
		&cls::PutPropertyByIndexImpl,			\
		&cls::DeletePropertyImpl,				\
		&cls::DeletePropertyByIndexImpl,		\
		&cls::DefineOwnPropertyImpl,			\
		&cls::GetDefaultValueImpl,				\
		&cls::ToThisObjectImpl,					\
		&cls::HasInstanceImpl,					\
		&cls::GetCallInfoImpl,					\
		&cls::GetConstructInfoImpl				\
	}

#define CREATE_EMPTY_CLASS_FUNCTION_TABLE(cls)	{					\
		&cls::GetClassNameImpl,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0	\
	}

#define JS_NO_CLASS_FUNCTIONS()
}

#endif