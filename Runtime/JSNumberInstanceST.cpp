#include "Runtime/JSNumberInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"

namespace MoEnjin
{
	JSNumberInstance* JSNumberInstance::Create(JSStackFrame* exec)
	{
		return JSNumberInstance::Create(exec, jsNumber(0));
	}

	JSNumberInstance* JSNumberInstance::Create(JSStackFrame* exec, JSValue value)
	{
		return new JSNumberInstance(exec->GetGlobalObject(), value, exec->GetGlobalObject()->GetNumberPrototype());
	}
}