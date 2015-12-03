#include "Runtime/JSBooleanInstance.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JSBooleanInstance* JSBooleanInstance::Create(JSStackFrame* exec)
	{
		return JSBooleanInstance::Create(exec, jsBool(mo_false));
	}

	JSBooleanInstance* JSBooleanInstance::Create(JSStackFrame* exec, JSValue value)
	{
		return new JSBooleanInstance(exec->GetGlobalObject(), value, exec->GetGlobalObject()->GetBooleanPrototype());
	}
}