#include "Runtime/JSStringPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSStringPrototype* JSStringPrototype::Create(JSGlobalObject* globalObject, JSValue prototype)
	{
		JSStringPrototype* stringProto = new JSStringPrototype(globalObject, prototype);
		stringProto->PutProperty(KnownIdentifiers::length, jsNumber(0), JSPropertyAttributeDontEnum | JSPropertyAttributeDontDelete | JSPropertyAttributeReadOnly);

		return stringProto;
	}
}