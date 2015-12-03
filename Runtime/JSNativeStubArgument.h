#ifndef MOENJIN_RUNTIME_JSNATIVESTUBARGUMENT_H
#define MOENJIN_RUNTIME_JSNATIVESTUBARGUMENT_H

#include "Platform/Configuration.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSFunction;
	class JSGlobalObject;
	class JSObject;
	class JSStackFrame;

	union JSNativeStubArgument {
		void* asPointer;
		JSValueImpl asValue;
		JSValueData asValueData;
		mo_int32 asInt32;

		mo_int32  Payload() const;
		mo_int32& Payload();
		mo_int32  Tag() const;
		mo_int32& Tag();

		JSValue ToValue() const;
		JSValue* ToValuePtr() const;
		JSStackFrame* ToStackFrame() const;
		JSGlobalObject* ToGlobalObject(mo_bool encoded = mo_false) const;
		JSObject* ToObject(mo_bool encoded = mo_false) const;
		JSFunction* ToFunction(mo_bool encoded = mo_false) const;
		NativeFunctionPtr ToNativeFunction() const;
		UTF8String ToUTF8String() const;
		mo_utf8stringptr ToCString() const;
		mo_int32 ToInt32(mo_bool encoded = mo_true) const;
		mo_bool ToBoolean(mo_bool encoded = mo_true) const;
	};

	mo_inline mo_int32 JSNativeStubArgument::ToInt32(mo_bool encoded) const
	{
		if(encoded)
			return Payload();

		return asInt32;
	}

	mo_inline mo_bool JSNativeStubArgument::ToBoolean(mo_bool encoded) const
	{
		if(encoded)
			return !!Payload();

		return (asInt32 != 0);
	}

	mo_inline JSValue JSNativeStubArgument::ToValue() const
	{
		return JSValue::Decode(asValue);
	}

	mo_inline JSValue* JSNativeStubArgument::ToValuePtr() const
	{
		return mo_cast<JSValue*>(asPointer);
	}

	mo_inline JSStackFrame* JSNativeStubArgument::ToStackFrame() const
	{
		return mo_cast<JSStackFrame*>(asPointer);
	}

	mo_inline JSGlobalObject* JSNativeStubArgument::ToGlobalObject(mo_bool encoded) const
	{
		if(encoded)
			return mo_cast_type<JSGlobalObject*>(JSValue::Decode(asValue).ToNativeObject());

		return mo_cast<JSGlobalObject*>(asPointer);
	}

	mo_inline JSObject* JSNativeStubArgument::ToObject(mo_bool encoded) const
	{
		if(encoded)
			return JSValue::Decode(asValue).ToNativeObject();

		return mo_cast<JSObject*>(asPointer);
	}

	mo_inline JSFunction* JSNativeStubArgument::ToFunction(mo_bool encoded) const
	{
		if(encoded)
			return mo_cast_type<JSFunction*>(JSValue::Decode(asValue).ToNativeObject());

		return mo_cast<JSFunction*>(asPointer);
	}

	mo_inline NativeFunctionPtr JSNativeStubArgument::ToNativeFunction() const
	{
		return mo_cast<NativeFunctionPtr>(asPointer);
	}

	mo_inline UTF8String JSNativeStubArgument::ToUTF8String() const
	{
		return UTF8String(ToCString());
	}

	mo_inline mo_utf8stringptr JSNativeStubArgument::ToCString() const
	{
		return mo_cast<mo_utf8stringptr>(asPointer);
	}

	mo_inline mo_int32  JSNativeStubArgument::Payload() const
	{
		return asValueData.asBits.payload;
	}

	mo_inline mo_int32& JSNativeStubArgument::Payload()
	{
		return asValueData.asBits.payload;
	}

	mo_inline mo_int32  JSNativeStubArgument::Tag() const
	{
		return asValueData.asBits.tag;
	}

	mo_inline mo_int32& JSNativeStubArgument::Tag()
	{
		return asValueData.asBits.tag;
	}
}

#endif