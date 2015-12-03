#include "Runtime/JSGetterSetter.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSGetterSetter, GetterSetter, JSObject);

	JSGetterSetter::JSGetterSetter(JSGlobalObject* globalObject)
		: JSObject(globalObject, JSObjectTypeGetterSetter, jsNull(), &ClassInfo)
		, mGetter(mo_null)
		, mSetter(mo_null) { }
}