#include "Runtime/JSNumberConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSNumberPrototype.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	static JSValue JSNumberConstructor_GetNaN(JSStackFrame*, JSValue, const UTF8String&);
	static JSValue JSNumberConstructor_GetNegativeInfinity(JSStackFrame*, JSValue, const UTF8String&);
	static JSValue JSNumberConstructor_GetPositiveInfinity(JSStackFrame*, JSValue, const UTF8String&);
	static JSValue JSNumberConstructor_GetMaxValue(JSStackFrame*, JSValue, const UTF8String&);
	static JSValue JSNumberConstructor_GetMinValue(JSStackFrame*, JSValue, const UTF8String&);
}

#include "Runtime/Tables/JSNumberConstructorTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSNumberConstructor, Function, JSConstructorFunction);

	JSNumberConstructor::JSNumberConstructor(JSGlobalObject* globalObject)
		: JSConstructorFunction(globalObject, globalObject->GetFunctionPrototype(), &ClassInfo, "Number") { }

	//
	// Number Constructor Functions
	//
	JSValue JSNumberConstructor_GetNaN(JSStackFrame* context, JSValue, const UTF8String&)
	{
		return JSValue();
	}

	JSValue JSNumberConstructor_GetNegativeInfinity(JSStackFrame* context, JSValue, const UTF8String&)
	{
		return JSValue();
	}

	JSValue JSNumberConstructor_GetPositiveInfinity(JSStackFrame* context, JSValue, const UTF8String&)
	{
		return JSValue();
	}

	JSValue JSNumberConstructor_GetMaxValue(JSStackFrame* context, JSValue, const UTF8String&)
	{
		return JSValue();
	}

	JSValue JSNumberConstructor_GetMinValue(JSStackFrame* context, JSValue, const UTF8String&)
	{
		return JSValue();
	}
}