#include "Runtime/JSNativeErrorPrototype.h"
#include "Runtime/JSNativeErrorConstructor.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSNativeErrorPrototype* JSNativeErrorPrototype::Create(JSStackFrame* exec, JSValue prototype, const UTF8String& name, JSNativeErrorConstructor* constructor)
	{
		JSNativeErrorPrototype* errorProto = new JSNativeErrorPrototype(exec->GetGlobalObject(), prototype);
		errorProto->Put(exec, KnownIdentifiers::name, JSString::FromString(exec->GetGlobalObject(), name), JSPropertyAttributeDontEnum);
		errorProto->Put(exec, KnownIdentifiers::message, jsEmptyString(exec->GetGlobalObject()), JSPropertyAttributeDontEnum);
		errorProto->Put(exec, KnownIdentifiers::constructor, constructor, JSPropertyAttributeDontEnum);

		return errorProto;
	}
}