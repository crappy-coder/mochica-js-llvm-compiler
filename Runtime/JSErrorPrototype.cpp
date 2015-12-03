#include "Runtime/JSErrorPrototype.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	static JSValueImpl JSErrorPrototype_ToString(JSStackFrame*);
}

#include "Runtime/Tables/JSErrorPrototypeTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSErrorPrototype, Error, JSErrorInstance);

	JSErrorPrototype::JSErrorPrototype(JSGlobalObject* globalObject, JSValue prototype)
		: JSErrorInstance(globalObject, prototype, &ClassInfo) { }

	mo_bool JSErrorPrototype::GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		return GetStaticFunctionDescriptor<JSErrorInstance>(exec, &JSErrorPrototypeTable, JSConvert::Cast<JSErrorPrototype*>(obj), propertyName, descriptor);
	}

	mo_bool JSErrorPrototype::GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		return GetStaticFunctionSlot<JSErrorInstance>(exec, &JSErrorPrototypeTable, JSConvert::Cast<JSErrorPrototype*>(obj), propertyName, slot);
	}

	//
	// Error Prototype Functions
	//
	JSValueImpl JSErrorPrototype_ToString(JSStackFrame* context)
	{
		return jsUndef();
	}
}