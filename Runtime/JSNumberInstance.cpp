#include "Runtime/JSNumberInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSNumberInstance, Number, JSObject);

	JSNumberInstance::JSNumberInstance(JSGlobalObject* globalObject, JSValue value, JSValue prototype)
		: JSObject(globalObject, JSObjectTypeObjectNumber, prototype, &ClassInfo)
		, mValue(value) { }

	JSNumberInstance::JSNumberInstance(JSGlobalObject* globalObject, JSValue value, JSValue prototype, const JSClassInfo* classInfo)
		: JSObject(globalObject, JSObjectTypeObjectNumber, prototype, classInfo)
		, mValue(value) { }
}