#include "Runtime/JSConstructorFunction.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	mo_bool JSConstructorFunction::IsCallableImpl()
	{
		return mo_true;
	}

	mo_bool JSConstructorFunction::ImplementsHasInstanceImpl()
	{
		return mo_true;
	}

	JSCallInfo JSConstructorFunction::GetCallInfoImpl(JSStackFrame* exec, JSObject* obj)
	{
		return jsNoCallInfo();
	}
}