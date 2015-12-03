#include "Runtime/JSGlobalObject.h"

namespace MoEnjin
{
	JSGlobalObject* JSGlobalObject::Create()
	{
		return JSGlobalObject::Create(jsNull(), &JSGlobalObject::ClassInfo);
	}

	JSGlobalObject* JSGlobalObject::Create(JSValue prototype)
	{
		return JSGlobalObject::Create(prototype, &JSGlobalObject::ClassInfo);
	}

	JSGlobalObject* JSGlobalObject::Create(JSValue prototype, const JSClassInfo* classInfo)
	{
		return new JSGlobalObject(prototype, classInfo);
	}
}