#include "Runtime/JSWrappedObject.h"

namespace MoEnjin
{
	JSWrappedObject::JSWrappedObject(JSGlobalObject* globalObject, JSObjectType type, JSValue prototype, JSValue initialValue)
		: JSObject(globalObject, type, prototype, &ClassInfo)
		, mValue(initialValue) { }

	JSWrappedObject::JSWrappedObject(JSGlobalObject* globalObject, JSObjectType type, JSValue prototype, const JSClassInfo* classInfo, JSValue initialValue)
		: JSObject(globalObject, type, prototype, classInfo)
		, mValue(initialValue) { }

	JSValue JSWrappedObject::GetValue() const
	{
		return mValue;
	}

	void JSWrappedObject::SetValue(JSValue value)
	{
		MO_ASSERT(value);
		MO_ASSERT(!value.IsObject());

		mValue = value;
	}
}