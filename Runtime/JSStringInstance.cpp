#include "Runtime/JSStringInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSStringInstance, String, JSObject);

	JSStringInstance::JSStringInstance(JSGlobalObject* globalObject, JSString* str, JSValue prototype)
		: JSObject(globalObject, JSObjectTypeObject, prototype, &ClassInfo)
		, mValue(str) { }

	JSStringInstance::JSStringInstance(JSGlobalObject* globalObject, JSString* str, JSValue prototype, const JSClassInfo* classInfo)
		: JSObject(globalObject, JSObjectTypeObject, prototype, classInfo)
		, mValue(str) { }
}