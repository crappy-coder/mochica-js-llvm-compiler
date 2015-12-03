#include "Runtime/JSFunctionConstructor.h"
#include "Runtime/JSFunctionPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSFunctionConstructor, Function, JSConstructorFunction); 

	JSFunctionConstructor::JSFunctionConstructor(JSGlobalObject* globalObject)
		: JSConstructorFunction(globalObject, globalObject->GetFunctionPrototype(), &ClassInfo, "Function") { }
}