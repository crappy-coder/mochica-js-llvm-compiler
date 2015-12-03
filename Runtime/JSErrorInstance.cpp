#include "Runtime/JSErrorInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSErrorInstance, Error, JSObject);

	JSErrorInstance::JSErrorInstance(JSGlobalObject* globalObject, JSValue prototype)
		: JSObject(globalObject, JSObjectTypeError, prototype, &ClassInfo)
		, mShowSourceInMessage(mo_false) { }

	JSErrorInstance::JSErrorInstance(JSGlobalObject* globalObject, JSValue prototype, const JSClassInfo* classInfo)
		: JSObject(globalObject, JSObjectTypeError, prototype, classInfo)
		, mShowSourceInMessage(mo_false) { }
}