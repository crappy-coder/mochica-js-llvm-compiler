#include "Runtime/JSValue.h"
#include "Runtime/JSBooleanInstance.h"
#include "Runtime/JSClassInfo.h"
#include "Runtime/JSConstructorFunction.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSExecutionEnvironment.h"
#include "Runtime/JSFunction.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSNumberInstance.h"
#include "Runtime/JSNumericStringCache.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSPropertySlot.h"
#include "Runtime/JSPutPropertySlot.h"
#include "Runtime/JSSmallStrings.h"
#include "Runtime/JSString.h"
#include "Runtime/JSStringInstance.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JSValue::JSValue(mo_double value)
	{
		const mo_int32 asInt32 = MO_CAST(mo_int32, value);

		if(asInt32 != value || (!asInt32 && signbit(value)))
		{
			*this = JSValue(JSValueDouble, value);
			return;
		}

		*this = JSValue(MO_CAST(mo_int32, value));
	}

	JSValue::JSValue(mo_bool value)
	{
		*this = (value ? JSValue(JSValueTrue) : JSValue(JSValueFalse));
	}

	JSValue::JSValue(mo_int8 value)
	{
		*this = JSValue(MO_CAST(mo_int32, value));
	}

	JSValue::JSValue(mo_uint8 value)
	{
		*this = JSValue(MO_CAST(mo_int32, value));
	}

	JSValue::JSValue(mo_int16 value)
	{
		*this = JSValue(MO_CAST(mo_int32, value));
	}

	JSValue::JSValue(mo_uint16 value)
	{
		*this = JSValue(MO_CAST(mo_int32, value));
	}

	JSValue::JSValue(mo_uint32 value)
	{
		if(mo_cast<mo_int32>(value) < 0)
		{
			*this = JSValue(JSValueDouble, static_cast<mo_double>(value));
			return;
		}

		*this = JSValue(mo_cast<mo_int32>(value));
	}

	JSValue::JSValue(mo_int64 value)
	{
		if(mo_cast<mo_int32>(value) != value)
		{
			*this = JSValue(JSValueDouble, static_cast<mo_double>(value));
			return;
		}

		*this = JSValue(mo_cast<mo_int32>(value));
	}

	JSValue::JSValue(mo_uint64 value)
	{
		if(mo_cast<mo_uint32>(value) != value)
		{
			*this = JSValue(JSValueDouble, static_cast<mo_double>(value));
			return;
		}

		*this = JSValue(mo_cast<mo_uint32>(value));
	}

	JSValue::JSValue(mo_long value)
	{
		if(mo_cast<mo_int32>(value) != value)
		{
			*this = JSValue(JSValueDouble, static_cast<mo_double>(value));
			return;
		}

		*this = JSValue(mo_cast<mo_int32>(value));
	}

	JSValue::JSValue(mo_ulong value)
	{
		if(mo_cast<mo_uint32>(value) != value)
		{
			*this = JSValue(JSValueDouble, static_cast<mo_double>(value));
			return;
		}

		*this = JSValue(mo_cast<mo_uint32>(value));
	}

	mo_uint32 JSValue::GetAsUInt32() const
	{
		MO_ASSERT(IsUInt32());
		return GetAsInt32();
	}

	mo_double JSValue::GetAsNumber() const
	{
		MO_ASSERT(IsNumber());
		return (IsInt32() ? GetAsInt32() : GetAsDouble());
	}
	
	mo_bool JSValue::IsUInt32() const
	{
		return (IsInt32() && GetAsInt32() >= 0);
	}

	mo_bool JSValue::IsFunction() const
	{
		return (IsNativeObject() && (GetAsNativeObject()->IsFunction() || GetAsNativeObject()->IsSubClassOf(&JSFunction::ClassInfo) || GetAsNativeObject()->IsSubClassOf(&JSConstructorFunction::ClassInfo)));
	}

	mo_bool JSValue::IsString() const
	{
		return (IsNativeString() || (IsNativeObject() && GetAsNativeObject()->IsString()));
	}

	mo_bool JSValue::IsPrimitive() const
	{
		return (!IsNativeObject() || IsString());
	}

	mo_bool JSValue::IsGetterOrSetter() const
	{
		return (IsNativeObject() && GetAsNativeObject()->IsGetterOrSetter());
	}

	mo_bool JSValue::IsCallable() const
	{
		if(!IsObject())
			return mo_false;

		return GetAsNativeObject()->IsCallable();
	}

	mo_bool JSValue::IsEnvironment() const
	{
		return (IsNativeObject() && GetAsNativeObject()->IsEnvironmentObject());
	}

	mo_bool JSValue::IsObject() const
	{
		return (IsNativeObject() && GetAsNativeObject()->IsObject());
	}

	mo_bool JSValue::IsSubClassOf(const JSClassInfo* info) const
	{
		return (IsNativeObject() && GetAsNativeObject()->IsSubClassOf(info));
	}

	mo_bool JSValue::TryGetAsUInt32(mo_uint32& outResult) const
	{
		if(IsInt32())
		{
			mo_int32 i = GetAsInt32();
			outResult = mo_cast<mo_uint32>(i);
			return (i >= 0);
		}

		if(IsDouble())
		{
			mo_double d = GetAsDouble();
			outResult = mo_cast<mo_uint32>(d);
			return (d == outResult);
		}

		return mo_false;
	}

	JSValue JSValue::Get(JSStackFrame* exec, const UTF8String& propertyName) const
	{
		if(IsNullOrUndefined())
		{
			jsThrowNativeError(kJSTypeError);
			return GetAsValue();
		}

		JSPropertySlot slot(GetAsValue());

		if(IsPrimitive())
		{
			JSObject* prototype = SynthesizePrototype(exec);

			if(!prototype->GetPropertySlot(exec, propertyName, slot))
				return jsUndef();

			return slot.GetValue(exec, propertyName);
		}

		JSObject* obj = GetAsNativeObject();

		if(obj->IsEnvironmentObject())
			return mo_cast<JSExecutionEnvironment*>(obj)->GetBindingValue(exec, propertyName);

		while(true)
		{
			if(obj->GetOwnPropertySlot(exec, propertyName, slot))
				return slot.GetValue(exec, propertyName);

			JSValue prototype = obj->GetPrototype();

			if(!prototype.IsObject())
				break;

			obj = prototype.ToNativeObject();
		}

		return jsUndef();
	}

	JSValue JSValue::Get(JSStackFrame* exec, mo_unsigned propertyName) const
	{
		if(IsNullOrUndefined())
		{
			jsThrowNativeError(kJSTypeError);
			return GetAsValue();
		}

		JSPropertySlot slot(GetAsValue());

		if(IsPrimitive())
		{
			JSObject* prototype = SynthesizePrototype(exec);

			if(!prototype->GetPropertySlot(exec, propertyName, slot))
				return jsUndef();

			return slot.GetValue(exec, propertyName);
		}

		JSObject* obj = GetAsNativeObject();

		while(true)
		{
			if(obj->GetOwnPropertySlotByIndex(exec, propertyName, slot))
				return slot.GetValue(exec, propertyName);

			JSValue prototype = obj->GetPrototype();

			if(!prototype.IsObject())
				break;

			obj = prototype.ToNativeObject();
		}

		return jsUndef();
	}

	// ECMA 5.1, §8.7.2
	void JSValue::Put(JSStackFrame* exec, const UTF8String& propertyName, JSValue value, mo_bool isStrictMode)
	{
		JSPutPropertySlot slot(isStrictMode);

		if(IsUndefined())
		{
			if(slot.IsStrictMode())
			{
				jsThrowNativeError(kJSReferenceError);
				return;
			}

			exec->GetGlobalObject()->PutProperty(exec, propertyName, value, mo_false, slot);
			return;
		}

		if(IsEnvironment())
		{
			JSExecutionEnvironment* env = mo_cast<JSExecutionEnvironment*>(GetAsNativeObject());
			env->SetMutableBinding(exec, propertyName, value, slot.IsStrictMode());
			return;
		}

		if(IsPrimitive())
		{
			PutToPrimitive(exec, propertyName, value, slot);
			return;
		}

		GetAsNativeObject()->PutProperty(exec, propertyName, value, mo_true, slot);
	}

	void JSValue::Put(JSStackFrame* exec, mo_unsigned propertyName, JSValue value, mo_bool isStrictMode)
	{
		if(IsPrimitive())
		{
			JSPutPropertySlot slot(isStrictMode);
			PutToPrimitive(exec, UTF8String::FromNumber(propertyName), value, slot);
			return;
		}

		GetAsNativeObject()->PutPropertyByIndex(exec, propertyName, value, isStrictMode);
	}

	void JSValue::PutToPrimitive(JSStackFrame* exec, const UTF8String& propertyName, JSValue value, JSPutPropertySlot& slot)
	{
		// TODO - need to implement
		jsThrowNativeError(kJSNotImplementedError);
	}

	JSObject* JSValue::SynthesizePrototype(JSStackFrame* exec) const
	{
		if(IsNativeObject())
		{
			MO_ASSERT(IsString());
			return exec->GetGlobalObject()->GetStringPrototype();
		}

		if(IsNumber())
			return exec->GetGlobalObject()->GetNumberPrototype();

		if(IsBoolean())
			return exec->GetGlobalObject()->GetBooleanPrototype();

		MO_ASSERT(IsNullOrUndefined());
		jsThrowNativeError(kJSDefaultError);

		// TODO - need to implement a JSGhostObject object
		return mo_null;
	}

	mo_double JSValue::ToDouble(JSStackFrame* exec) const
	{
		return ToNumber(exec);
	}

	// ECMA 5.1, §9.3
	mo_double JSValue::ToNumber(JSStackFrame* exec) const
	{
		if(IsInt32())
			return GetAsInt32();

		if(IsDouble())
			return GetAsDouble();

		if(IsNativeObject())
			return GetAsNativeObject()->ToNumber(exec);

		if(IsNativeString())
			return JSConvert::ToNumber(UTF8String(GetAsString()));

		if(IsTrue())
			return 1.0;

		// [undefined] returns NaN, otherwise [null] and [false] both return zero
		return IsUndefined() ? std::numeric_limits<mo_double>::quiet_NaN() : 0;
	}

	mo_int32 JSValue::ToInt32(JSStackFrame* exec) const
	{
		if(IsInt32())
			return GetAsInt32();

		return JSConvert::ToInt32(ToNumber(exec));
	}

	mo_uint32 JSValue::ToUInt32(JSStackFrame* exec) const
	{
		return ToInt32(exec);
	}

	// ECMA 5.1, §9.2
	mo_bool JSValue::ToBoolean(JSStackFrame* exec) const
	{
		if(IsInt32())
			return (GetAsInt32() != 0);

		if(IsDouble())
			return (GetAsDouble() > 0.0 || GetAsDouble() < 0.0);

		if(IsNativeObject())
			return GetAsNativeObject()->ToBoolean(exec);

		if(IsNativeString())
			return (GetAsString()[0] != '\0');

		// only returns true if this is a boolean value and
		// it's payload is true, otherwise returns false 
		// for [false], [null] and [undefined]
		return IsTrue();
	}

	// ECMA 5.1, §9.8
	JSString* JSValue::ToString(JSStackFrame* exec) const
	{
		JSGlobalObject* globalObject = exec->GetGlobalObject();

		if(IsNativeObject())
			return GetAsNativeObject()->ToString(exec);

		if(IsNativeString())
			return JSString::Create(globalObject, UTF8String(GetAsString()));

		if(IsInt32())
			return JSString::FromString(globalObject, globalObject->NumericStrings->Add(GetAsInt32()));

		if(IsDouble())
			return JSString::FromString(globalObject, globalObject->NumericStrings->Add(GetAsDouble()));

		if(IsTrue())
			return globalObject->SmallStrings->GetTrueKeyword(globalObject);

		if(IsFalse())
			return globalObject->SmallStrings->GetFalseKeyword(globalObject);

		if(IsNull())
			return globalObject->SmallStrings->GetNullKeyword(globalObject);

		// must be undefined or something bad happened...
		return globalObject->SmallStrings->GetUndefinedKeyword(globalObject);
	}

	// TODO - if we are a native string, then create a JSString and return that?
	//
	JSNativeObject* JSValue::ToNativeObject() const
	{
		return (IsNativeObject() ? GetAsNativeObject() : mo_null);
	}

	UTF8String JSValue::ToUTF8String() const
	{
		return (IsNativeString() ? UTF8String(GetAsString()) : UTF8String::MakeNull());
	}

	JSObject* JSValue::ToObject(JSStackFrame* exec) const
	{
		if(IsNativeObject())
			return GetAsNativeObject()->ToObject(exec);

		if(IsNativeString())
			return JSStringInstance::Create(exec, JSString::Create(exec->GetGlobalObject(), UTF8String(GetAsString())));

		if(IsInt32() || IsDouble())
			return JSNumberInstance::Create(exec, GetAsValue());

		if(IsBoolean())
			return JSBooleanInstance::Create(exec, GetAsValue());

		MO_ASSERT(IsNullOrUndefined());

		// TODO - throw NotAnObjectError
		jsThrowNativeError(kJSTypeError);

		return JSObject::Create(exec); // TODO - return a new JSNotAnObject
	}

	JSObject* JSValue::ToThisObject(JSStackFrame* exec) const
	{
		if(IsNativeObject())
			return GetAsNativeObject()->ToThisObject(exec);
		
		if(IsNativeString())
			return JSStringInstance::Create(exec, JSString::Create(exec->GetGlobalObject(), UTF8String(GetAsString())));

		if(IsInt32() || IsDouble())
			return JSNumberInstance::Create(exec, GetAsValue());

		if(IsBoolean())
			return JSBooleanInstance::Create(exec, GetAsValue());

		MO_ASSERT(IsNullOrUndefined());

		return exec->GetGlobalThis();
	}

	JSValue JSValue::ToPrimitive(JSStackFrame* exec, JSTypePreference preference) const
	{
		return (IsNativeObject() ? GetAsNativeObject()->ToPrimitive(exec, preference) : GetAsValue());
	}

	JSValue JSValue::ToPrimitiveNumber(JSStackFrame* exec, mo_double& number) const
	{
		if(IsInt32())
		{
			number = GetAsInt32();
			return *this;
		}

		if(IsDouble())
		{
			number = GetAsDouble();
			return *this;
		}

		if(IsNativeObject())
			return GetAsNativeObject()->ToPrimitiveNumber(exec, number);

		if(IsTrue())
		{
			number = 1.0;
			return *this;
		}

		if(IsFalse())
		{
			number = 0.0;
			return *this;
		}

		MO_ASSERT(IsUndefined());
		
		number = std::numeric_limits<mo_double>::quiet_NaN();
		return *this;
	}

	JSValue JSValue::ToTypeString(JSStackFrame* exec) const
	{
		JSGlobalObject* globalObject = exec->GetGlobalObject();

		if(IsUndefined())
			return globalObject->SmallStrings->GetUndefinedKeyword(globalObject);
		
		if(IsString())
			return globalObject->SmallStrings->GetStringKeyword(globalObject);

		if(IsBoolean())
			return globalObject->SmallStrings->GetBooleanKeyword(globalObject);

		if(IsNumber())
			return globalObject->SmallStrings->GetNumberKeyword(globalObject);

		if(IsCallable())
			return globalObject->SmallStrings->GetFunctionKeyword(globalObject);

		return globalObject->SmallStrings->GetObjectKeyword(globalObject);
	}

	JSCallInfo JSValue::GetCallInfo(JSStackFrame* exec)
	{
		if(!IsNativeObject())
			return jsNoCallInfo();

		return GetAsNativeObject()->GetCallInfo(exec);
	}

	JSCallInfo JSValue::GetConstructInfo(JSStackFrame* exec)
	{
		if(!IsNativeObject())
			return jsNoCallInfo();

		return GetAsNativeObject()->GetConstructInfo(exec);
	}

#if defined(MO_PLATFORM_USE_JSVALUE_32)
	JSValue::JSValue()
	{
		mData.asBits.tag = JSValueDataEmptyTag;
		mData.asBits.payload = 0;
	}

	JSValue::JSValue(JSValueNullTag)
	{
		mData.asBits.tag = JSValueDataNullTag;
		mData.asBits.payload = 0;
	}

	JSValue::JSValue(JSValueUndefinedTag)
	{
		mData.asBits.tag = JSValueDataUndefinedTag;
		mData.asBits.payload = 0;
	}

	JSValue::JSValue(JSValueTrueTag)
	{
		mData.asBits.tag = JSValueDataBooleanTag;
		mData.asBits.payload = 1;
	}

	JSValue::JSValue(JSValueFalseTag)
	{
		mData.asBits.tag = JSValueDataBooleanTag;
		mData.asBits.payload = 0;
	}

	JSValue::JSValue(JSValueDoubleTag, mo_double value)
	{
		mData.asDouble = value;
	}

	JSValue::JSValue(JSNativeObject* obj)
	{
		mData.asBits.tag = (obj ? JSValueDataObjectTag : JSValueDataEmptyTag);
		mData.asBits.payload = reinterpret_cast<mo_int32>(obj);
	}

	JSValue::JSValue(const JSNativeObject* obj)
	{
		mData.asBits.tag = (obj ? JSValueDataObjectTag : JSValueDataEmptyTag);
		mData.asBits.payload = reinterpret_cast<mo_int32>(const_cast<JSNativeObject*>(obj));
	}

	JSValue::JSValue(mo_utf8stringptr value)
	{
		mData.asBits.tag = (value ? JSValueDataStringTag : JSValueDataEmptyTag);
		mData.asBits.payload = reinterpret_cast<mo_int32>(value);
	}

	JSValue::JSValue(const mo_utf8stringptr value)
	{
		mData.asBits.tag = (value ? JSValueDataStringTag : JSValueDataEmptyTag);
		mData.asBits.payload = reinterpret_cast<mo_int32>(const_cast<mo_utf8stringptr>(value));
	}

	JSValue::JSValue(mo_int32 value)
	{
		mData.asBits.tag = JSValueDataIntTag;
		mData.asBits.payload = value;
	}

	mo_int32 JSValue::GetAsInt32() const
	{
		MO_ASSERT(IsInt32());
		return mData.asBits.payload;
	}

	mo_double JSValue::GetAsDouble() const
	{
		MO_ASSERT(IsDouble());
		return mData.asDouble;
	}

	mo_bool JSValue::GetAsBoolean() const
	{
		MO_ASSERT(IsBoolean());
		return (mData.asBits.payload != 0);
	}

	mo_utf8stringptr JSValue::GetAsString() const
	{
		MO_ASSERT(IsNativeString());
		return mo_cast_type<mo_utf8stringptr>(mData.asBits.payload);
	}

	JSNativeObject* JSValue::GetAsNativeObject() const
	{
		MO_ASSERT(IsNativeObject());
		return mo_cast_type<JSNativeObject*>(mData.asBits.payload);
	}

	mo_bool JSValue::IsInt32() const
	{
		return (mData.asBits.tag == JSValueDataIntTag);
	}

	mo_bool JSValue::IsDouble() const
	{
		return (mData.asBits.tag > 0 || mData.asBits.tag < JSValueDataLowestTag);
	}

	mo_bool JSValue::IsTrue() const
	{
		return (mData.asBits.tag == JSValueDataBooleanTag && mData.asBits.payload != 0);
	}

	mo_bool JSValue::IsFalse() const
	{
		return (mData.asBits.tag == JSValueDataBooleanTag && mData.asBits.payload == 0);
	}

	mo_bool JSValue::IsEmpty() const
	{
		return (mData.asBits.tag == JSValueDataEmptyTag);
	}

	mo_bool JSValue::IsUndefined() const
	{
		return (mData.asBits.tag == JSValueDataUndefinedTag);
	}

	mo_bool JSValue::IsNull() const
	{
		return (mData.asBits.tag == JSValueDataNullTag);
	}

	mo_bool JSValue::IsNullOrUndefined() const
	{
		return (IsNull() || IsUndefined());
	}

	mo_bool JSValue::IsBoolean() const
	{
		return (IsTrue() || IsFalse());
	}

	mo_bool JSValue::IsNumber() const
	{
		return (IsInt32() || IsDouble());
	}

	mo_bool JSValue::IsNativeString() const
	{
		return (mData.asBits.tag == JSValueDataStringTag);
	}

	mo_bool JSValue::IsNativeObject() const
	{
		return (mData.asBits.tag == JSValueDataObjectTag);
	}
#elif defined(MO_PLATFORM_USE_JSVALUE_64)
	JSValue::JSValue()
	{
		mData.asInt64 = ValueEmpty;
	}

	JSValue::JSValue(JSValueNullTag)
	{
		mData.asInt64 = ValueNull;
	}

	JSValue::JSValue(JSValueUndefinedTag)
	{
		mData.asInt64 = ValueUndefined;
	}

	JSValue::JSValue(JSValueTrueTag)
	{
		mData.asInt64 = ValueTrue;
	}

	JSValue::JSValue(JSValueFalseTag)
	{
		mData.asInt64 = ValueFalse;
	}

	JSValue::JSValue(JSValueDoubleTag, mo_double value)
	{
		mData.asInt64 = static_cast<intptr_t>(value) + DoubleEncodeOffset;
	}

	JSValue::JSValue(JSObject* ptr)
	{
		mData.asPtr = ptr;
	}

	JSValue::JSValue(const JSNativeObject* ptr)
	{
		mData.asPtr = const_cast<JSNativeObject*>(ptr);
	}

	JSValue::JSValue(mo_int32 value)
	{
		mData.asInt64 = TagTypeNumber | static_cast<mo_uint32>(value);
	}

	mo_int32 JSValue::GetAsInt32() const
	{
		MO_ASSERT(IsInt32());
		return static_cast<mo_int32>(mData.asInt64);
	}

	mo_double JSValue::GetAsDouble() const
	{
		MO_ASSERT(IsDouble());
		return bitwise_cast<mo_double>((intptr_t)(mData.asInt64 - DoubleEncodeOffset));
	}

	mo_bool JSValue::GetAsBoolean() const
	{
		MO_ASSERT(IsBoolean());
		return (GetAsValue() == JSValue(JSValueTrue));
	}

	JSNativeObject* JSValue::GetAsNativeObject() const
	{
		MO_ASSERT(IsNativeObject());
		return mData.asPtr;
	}

	mo_bool JSValue::IsInt32() const
	{
		return ((mData.asInt64 & TagTypeNumber) == TagTypeNumber);
	}

	mo_bool JSValue::IsDouble() const
	{
		return (IsNumber() && !IsInt32());
	}

	mo_bool JSValue::IsTrue() const
	{
		return (GetAsValue() == JSValue(JSValueTrue));
	}

	mo_bool JSValue::IsFalse() const
	{
		return (GetAsValue() == JSValue(JSValueFalse));
	}

	mo_bool JSValue::IsEmpty() const
	{
		return (mData.asInt64 == ValueEmpty);
	}

	mo_bool JSValue::IsUndefined() const
	{
		return (GetAsValue() == JSValue(JSValueUndefined));
	}

	mo_bool JSValue::IsNull() const
	{
		return (GetAsValue() == JSValue(JSValueNull));
	}

	mo_bool JSValue::IsNullOrUndefined() const
	{
		return ((mData.asInt64 & ~TagBitUndefined) == ValueNull);
	}

	mo_bool JSValue::IsBoolean() const
	{
		return ((mData.asInt64 & ~1) == ValueFalse);
	}

	mo_bool JSValue::IsNumber() const
	{
		return (mData.asInt64 & TagTypeNumber);
	}

	mo_bool JSValue::IsNativeObject() const
	{
		return !(mData.asInt64 & TagMask);
	}
#endif
}