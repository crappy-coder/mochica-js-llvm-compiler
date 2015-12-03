#include "Runtime/JSErrorInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSErrorInstance* JSErrorInstance::Create(JSStackFrame* exec, JSValue message)
	{
		return JSErrorInstance::Create(exec, message.IsUndefined() ? UTF8String::MakeNull() : message.ToString(exec)->GetValue());
	}

	JSErrorInstance* JSErrorInstance::Create(JSStackFrame* exec, JSValue prototype, JSValue message)
	{
		return JSErrorInstance::Create(exec, prototype, message.IsUndefined() ? UTF8String::MakeNull() : message.ToString(exec)->GetValue());
	}

	JSErrorInstance* JSErrorInstance::Create(JSStackFrame* exec, const UTF8String& message)
	{
		return JSErrorInstance::Create(exec, exec->GetGlobalObject()->GetErrorPrototype(), message);
	}

	JSErrorInstance* JSErrorInstance::Create(JSStackFrame* exec, JSValue prototype, const UTF8String& message)
	{
		JSErrorInstance* errorInstance = new JSErrorInstance(exec->GetGlobalObject(), prototype);

		if(!message.IsNull())
			errorInstance->Put(exec, KnownIdentifiers::message, JSString::FromString(exec->GetGlobalObject(), message), JSPropertyAttributeDontEnum);

		return errorInstance;
	}
}