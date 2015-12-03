#include "Runtime/JSString.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStringRope.h"
#include "Runtime/JSStringInstance.h"

namespace MoEnjin
{
	JS_DEFINE_AS_NAKED_SUBCLASS(JSString, String, JSObject);

	JSString::JSString(JSGlobalObject* globalObject)
		: JSObject(globalObject, JSObjectTypeString, jsNull(), &ClassInfo)
		, mValue(mo_null)
		, mLength(0) { }

	JSString::JSString(JSGlobalObject* globalObject, const UTF8String& value)
		: JSObject(globalObject, JSObjectTypeString, jsNull(), &ClassInfo)
		, mValue(value)
		, mLength(value.GetLength()) { }

	const UTF8String& JSString::GetValue() const
	{
		if(IsRope())
			mo_cast<const JSStringRope*>(this)->Resolve();

		return mValue;
	}

	JSString* JSString::GetChar(mo_unsigned index)
	{
		MO_ASSERT(index < mLength);

		if(IsRope())
			return mo_cast<JSStringRope*>(this)->GetCharInRope(index);

		MO_ASSERT(index < mValue.GetLength());

		return JSString::FromSubstring(GetGlobalObject(), mValue, index);
	}

	mo_bool JSString::ToBoolean(JSStackFrame* exec) const
	{
		return (mLength > 0);
	}

	mo_double JSString::ToNumber(JSStackFrame* exec) const
	{
		return JSConvert::ToNumber(GetValue());
	}

	JSObject* JSString::ToObject(JSStackFrame* exec) const
	{
		return JSStringInstance::Create(exec, mo_cast_const<JSString*>(this));		
	}

	JSValue JSString::ToPrimitiveNumber(JSStackFrame* exec, mo_double& number) const
	{
		number = ToNumber(exec);
		return JSValue(this);
	}
}