#include "Runtime/JSStringConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStringPrototype.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	static JSValueImpl JSStringConstructor_FromCharCode(JSStackFrame*);
}

#include "Runtime/Tables/JSStringConstructorTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSStringConstructor, Function, JSConstructorFunction);

	JSStringConstructor::JSStringConstructor(JSGlobalObject* globalObject)
		: JSConstructorFunction(globalObject, globalObject->GetFunctionPrototype(), &ClassInfo, "String") { }

	//
	// String Constructor Functions
	//
	JSValueImpl JSStringConstructor_FromCharCode(JSStackFrame* exec)
	{
		return jsUndef();
	}
}