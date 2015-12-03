#include "Runtime/JSErrorConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSErrorConstructor, Function, JSConstructorFunction);

	JSErrorConstructor::JSErrorConstructor(JSGlobalObject* globalObject)
		: JSConstructorFunction(globalObject, globalObject->GetFunctionPrototype(), &ClassInfo, "Error") { }
}