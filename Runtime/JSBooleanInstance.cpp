#include "Runtime/JSBooleanInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSBooleanInstance, Boolean, JSObject);

	JSBooleanInstance::JSBooleanInstance(JSGlobalObject* globalObject, JSValue value, JSValue prototype)
		: JSObject(globalObject, JSObjectTypeObjectBoolean, prototype, &ClassInfo)
		, mValue(value) { }

	JSBooleanInstance::JSBooleanInstance(JSGlobalObject* globalObject, JSValue value, JSValue prototype, const JSClassInfo* classInfo)
		: JSObject(globalObject, JSObjectTypeObjectBoolean, prototype, classInfo)
		, mValue(value) { }
}