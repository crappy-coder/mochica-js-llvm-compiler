#include "Runtime/JSFunctionPrototype.h"
#include "Runtime/JSGlobalObject.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSFunctionPrototype, Function, JSConstructorFunction);

	JSFunctionPrototype::JSFunctionPrototype(JSGlobalObject* globalObject, JSValue prototype)
		: JSConstructorFunction(globalObject, prototype, &ClassInfo, "Function") { }

	JSFunctionPrototype* JSFunctionPrototype::Create(JSGlobalObject* globalObject, JSValue prototype)
	{
		return new JSFunctionPrototype(globalObject, prototype);
	}
}