#include "Runtime/JSStringInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"

namespace MoEnjin
{
	JSStringInstance* JSStringInstance::Create(JSStackFrame* exec)
	{
		return JSStringInstance::Create(exec, JSString::FromString(exec->GetGlobalObject(), UTF8String::MakeEmpty()));
	}

	JSStringInstance* JSStringInstance::Create(JSStackFrame* exec, JSString* str)
	{
		return new JSStringInstance(exec->GetGlobalObject(), str, exec->GetGlobalObject()->GetStringPrototype());
	}
}