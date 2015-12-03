#include "Runtime/JSExecutionEnvironment.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_BASE_CLASS(JSExecutionEnvironment, Object);

	JSExecutionEnvironment::JSExecutionEnvironment(JSGlobalObject* globalObject, JSExecutionEnvironment* parent)
		: JSObject(globalObject, JSObjectTypeEnvironment, jsNull(), &ClassInfo)
		, mParent(parent) { }

	JSValue JSExecutionEnvironment::GetIdentifierReference(JSStackFrame* exec, const UTF8String& name, mo_bool strictMode)
	{
		if(HasBinding(exec, name))
			return jsObject(this);

		if(mParent)
			return mParent->GetIdentifierReference(exec, name, strictMode);

		return jsUndef();
	}
}