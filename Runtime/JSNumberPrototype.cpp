#include "Runtime/JSNumberPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	static JSValueImpl JSNumberPrototype_ToString(JSStackFrame*);
	static JSValueImpl JSNumberPrototype_ToLocaleString(JSStackFrame*);
	static JSValueImpl JSNumberPrototype_ValueOf(JSStackFrame*);
	static JSValueImpl JSNumberPrototype_ToFixed(JSStackFrame*);
	static JSValueImpl JSNumberPrototype_ToExponential(JSStackFrame*);
	static JSValueImpl JSNumberPrototype_ToPrecision(JSStackFrame*);
}

#include "Runtime/Tables/JSNumberPrototypeTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSNumberPrototype, Number, JSNumberInstance);

	JSNumberPrototype::JSNumberPrototype(JSGlobalObject* globalObject, JSValue prototype)
		: JSNumberInstance(globalObject, jsNumber(0), prototype, &ClassInfo) { }

	//
	// Number Prototype Functions
	//
	JSValueImpl JSNumberPrototype_ToString(JSStackFrame* context)
	{
		return JSValue::Encode(JSValue());
	}

	JSValueImpl JSNumberPrototype_ToLocaleString(JSStackFrame* context)
	{
		return JSValue::Encode(JSValue());
	}

	JSValueImpl JSNumberPrototype_ValueOf(JSStackFrame* context)
	{
		return JSValue::Encode(JSValue());
	}

	JSValueImpl JSNumberPrototype_ToFixed(JSStackFrame* context)
	{
		return JSValue::Encode(JSValue());
	}

	JSValueImpl JSNumberPrototype_ToExponential(JSStackFrame* context)
	{
		return JSValue::Encode(JSValue());
	}

	JSValueImpl JSNumberPrototype_ToPrecision(JSStackFrame* context)
	{
		return JSValue::Encode(JSValue());
	}

}