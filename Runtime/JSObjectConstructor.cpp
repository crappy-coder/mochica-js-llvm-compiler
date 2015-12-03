#include "Runtime/JSObjectConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSObjectPrototype.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	static JSValueImpl JSObjectConstructor_Create(JSStackFrame*);
}

#include "Runtime/Tables/JSObjectConstructorTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSObjectConstructor, Function, JSConstructorFunction);

	JSObjectConstructor::JSObjectConstructor(JSGlobalObject* globalObject)
		: JSConstructorFunction(globalObject, globalObject->GetFunctionPrototype(), &ClassInfo, "Object") { }

	JSValueImpl JSObjectConstructor_Create(JSStackFrame* exec)
	{
		return jsUndef();
	}
}