#include "Runtime/JSString.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSSmallStrings.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	JSString* JSString::Create(JSGlobalObject* globalObject)
	{
		return new JSString(globalObject);
	}

	JSString* JSString::Create(JSGlobalObject* globalObject, const UTF8String& str)
	{
		return new JSString(globalObject, str);
	}

	JSString* JSString::CreateEmpty(JSGlobalObject* globalObject)
	{
		return globalObject->SmallStrings->EmptyString(globalObject);
	}

	JSString* JSString::FromString(JSGlobalObject* globalObject, const UTF8String& str)
	{
		if(str.IsEmpty())
			return globalObject->SmallStrings->EmptyString(globalObject);

		if(str.GetLength() == 1)
		{
			mo_utf8char ch = str[0];

			if(ch <= JSSmallStrings::MaxCharString)
				return globalObject->SmallStrings->CharacterString(globalObject, ch);
		}

		return JSString::Create(globalObject, str);
	}

	JSString* JSString::FromString(JSGlobalObject* globalObject, JSString* str)
	{
		return JSString::FromString(globalObject, str->GetValue());
	}

	JSString* JSString::FromSubstring(JSGlobalObject* globalObject, const UTF8String& str, mo_unsigned offset, mo_unsigned length)
	{
		MO_ASSERT(offset <= str.GetLength());
		MO_ASSERT(length <= str.GetLength());
		MO_ASSERT((offset + length) <= str.GetLength());

		if(!length)
			return globalObject->SmallStrings->EmptyString(globalObject);

		if(length == 1)
		{
			mo_utf8char ch = str[offset];

			if(ch <= JSSmallStrings::MaxCharString)
				return globalObject->SmallStrings->CharacterString(globalObject, ch);
		}

		return JSString::Create(globalObject, str.Substring(offset, length));
	}

	JSString* JSString::FromSubstring(JSGlobalObject* globalObject, JSString* str, mo_unsigned offset, mo_unsigned length)
	{
		return JSString::FromSubstring(globalObject, str->GetValue(), offset, length);
	}

	JSString* JSString::FromCharacter(JSGlobalObject* globalObject, mo_utf8char ch)
	{
		if(ch <= JSSmallStrings::MaxCharString)
			return globalObject->SmallStrings->CharacterString(globalObject, ch);

		return JSString::Create(globalObject, UTF8String(&ch, 1));
	}

	mo_bool JSString::GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		JSString* me = JSConvert::Cast<JSString*>(obj);

		// get the length of this string
		if(propertyName == KnownIdentifiers::length)
		{
			descriptor.Set(jsNumber(me->mLength), JS_PROP_ATTR_FIXED);
			return mo_true;
		}

		// check if the property name is an index into
		// this string and return that character if so
		mo_unsigned idx = 0;

		if(propertyName.ToUInt32(idx) && idx < me->mLength)
		{
			descriptor.Set(me->GetChar(idx), JS_PROP_ATTR(DontDelete) | JS_PROP_ATTR(ReadOnly));
			return mo_true;
		}

		return mo_false;
	}

	mo_bool JSString::GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		JSString* me = JSConvert::Cast<JSString*>(obj);

		// get the length of this string
		if(propertyName == KnownIdentifiers::length)
		{
			slot.SetValue(jsNumber(me->mLength));
			return mo_true;
		}

		// check if the property name is an index into
		// this string and return that character if so
		mo_unsigned idx = 0;

		if(propertyName.ToUInt32(idx) && idx < me->mLength)
		{
			slot.SetValue(me->GetChar(idx));
			return mo_true;
		}


		// otherwise check the string prototype
		JSObject* protoObject = mo_null;

		slot.SetBase(me);

		for(JSValue proto = exec->GetGlobalObject()->GetStringPrototype(); !proto.IsNull(); proto = protoObject->GetPrototype())
		{
			protoObject = proto.ToNativeObject();

			if(protoObject->GetOwnPropertySlot(exec, propertyName, slot))
				return mo_true;
		}

		slot.SetUndefined();
		return mo_true;
	}

	mo_bool JSString::GetOwnPropertySlotByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, JSPropertySlot& slot)
	{
		JSString* me = JSConvert::Cast<JSString*>(obj);

		if(propertyName < me->mLength)
		{
			slot.SetValue(me->GetChar(propertyName));
			return mo_true;
		}

		return JSObject::GetOwnPropertySlotImpl(exec, obj, UTF8String::FromNumber(propertyName), slot);
	}

	JSObject* JSString::ToThisObjectImpl(JSStackFrame* exec, JSObject* obj)
	{
		return JSStringInstance::Create(exec, JSConvert::Cast<JSString*>(obj));
	}
}