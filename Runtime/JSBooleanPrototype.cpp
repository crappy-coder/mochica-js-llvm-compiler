#include "Runtime/JSBooleanPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	static JSValueImpl JSBooleanPrototype_ToString(JSStackFrame*);
	static JSValueImpl JSBooleanPrototype_ValueOf(JSStackFrame*);
}

#include "Runtime/Tables/JSBooleanPrototypeTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSBooleanPrototype, Boolean, JSBooleanInstance);

	JSBooleanPrototype::JSBooleanPrototype(JSGlobalObject* globalObject, JSValue prototype)
		: JSBooleanInstance(globalObject, jsBool(mo_false), prototype, &ClassInfo) { }

	//
	// Boolean Prototype Functions
	//
	JSValueImpl JSBooleanPrototype_ToString(JSStackFrame* context)
	{
		return jsUndef();
	}

	JSValueImpl JSBooleanPrototype_ValueOf(JSStackFrame* context)
	{
		return jsUndef();
	}
}