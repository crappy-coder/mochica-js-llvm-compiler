#ifndef MOENJIN_RUNTIME_JSVALUE_H
#define MOENJIN_RUNTIME_JSVALUE_H

#include "Platform/Configuration.h"
#include "Runtime/JSTypePreference.h"
#include "Runtime/JSValueData.h"
#include "Runtime/JSValueTags.h"

namespace MoEnjin
{
	struct JSCallInfo;
	struct JSClassInfo;

	class JSObject;
	class JSPropertySlot;
	class JSPutPropertySlot;
	class JSStackFrame;
	class JSString;


	typedef JSValueImpl (*NativeFunctionPtr)(JSStackFrame*);

	#define NATIVE_FUNCTION_PTR(f,c)	((mo_intptr)mo_cast<NativeFunctionPtr>(f)), ((mo_intptr)c)
	#define NATIVE_FUNCTION_ARGC(c)		((mo_intptr)c)

	class JSValue
	{
		public:
			static JSValueImpl Encode(JSValue value);
			static JSValue Decode(JSValueImpl encValue);

			static JSValue MakeEmpty();
			static JSValue MakeUndefined();
			static JSValue MakeNull();
			static JSValue MakeTrue();
			static JSValue MakeFalse();
			static JSValue MakeBoolean(mo_bool value);
			static JSValue MakeDouble(mo_double value);
			static JSValue MakeNumber(mo_double value);
			static JSValue MakeNumber(mo_int8 value);
			static JSValue MakeNumber(mo_int16 value);
			static JSValue MakeNumber(mo_int32 value);
			static JSValue MakeNumber(mo_int64 value);
			static JSValue MakeNumber(mo_long value);
			static JSValue MakeNumber(mo_uint8 value);
			static JSValue MakeNumber(mo_uint16 value);
			static JSValue MakeNumber(mo_uint32 value);
			static JSValue MakeNumber(mo_uint64 value);
			static JSValue MakeNumber(mo_ulong value);
			static JSValue MakeString(const UTF8String& value);

			// ECMA 5.1 - 11.9.1
			static mo_bool AreEqual(JSValue a, JSValue b);

			// ECMA 5.1 - 11.9.4
			static mo_bool AreStrictEqual(JSValue a, JSValue b);

			// ECMA 5.1 - 9.12
			static mo_bool AreSameValue(JSValue a, JSValue b);

		public:
			JSValue();
			JSValue(JSValueNullTag);
			JSValue(JSValueUndefinedTag);
			JSValue(JSValueTrueTag);
			JSValue(JSValueFalseTag);
			JSValue(JSValueDoubleTag, mo_double value);
			JSValue(JSObject* obj);
			JSValue(const JSObject* obj);
			JSValue(mo_utf8stringptr value);
			JSValue(const mo_utf8stringptr value);

			explicit JSValue(mo_double value);
			explicit JSValue(mo_bool value);

			explicit JSValue(mo_int8 value);
			explicit JSValue(mo_int16 value);
			explicit JSValue(mo_int32 value);
			explicit JSValue(mo_int64 value);
			explicit JSValue(mo_long value);

			explicit JSValue(mo_uint8 value);
			explicit JSValue(mo_uint16 value);
			explicit JSValue(mo_uint32 value);
			explicit JSValue(mo_uint64 value);
			explicit JSValue(mo_ulong value);

			mo_int32 GetAsInt32() const;
			mo_uint32 GetAsUInt32() const;
			mo_double GetAsDouble() const;
			mo_double GetAsNumber() const;
			mo_bool GetAsBoolean() const;
			mo_utf8stringptr GetAsString() const;
			JSObject* GetAsNativeObject() const;

			mo_double ToDouble(JSStackFrame* exec) const;
			mo_double ToNumber(JSStackFrame* exec) const;
			mo_int32 ToInt32(JSStackFrame* exec) const;
			mo_uint32 ToUInt32(JSStackFrame* exec) const;
			mo_bool ToBoolean(JSStackFrame* exec) const;
			JSString* ToString(JSStackFrame* exec) const;
			JSObject* ToObject(JSStackFrame* exec) const;
			JSObject* ToThisObject(JSStackFrame* exec) const;
			JSObject* ToNativeObject() const;
			UTF8String ToUTF8String() const;

			JSValue ToPrimitive(JSStackFrame* exec, JSTypePreference preference = JSTypeNoPreference) const;
			JSValue ToPrimitiveNumber(JSStackFrame* exec, mo_double& number) const;

			JSValue ToTypeString(JSStackFrame* exec) const;

			mo_bool IsInt32() const;
			mo_bool IsUInt32() const;
			mo_bool IsDouble() const;
			mo_bool IsTrue() const;
			mo_bool IsFalse() const;
			mo_bool IsEmpty() const;
			mo_bool IsFunction() const;
			mo_bool IsUndefined() const;
			mo_bool IsNull() const;
			mo_bool IsNullOrUndefined() const;
			mo_bool IsBoolean() const;
			mo_bool IsNumber() const;
			mo_bool IsString() const;
			mo_bool IsNativeString() const;
			mo_bool IsPrimitive() const;
			mo_bool IsGetterOrSetter() const;
			mo_bool IsCallable() const;
			mo_bool IsEnvironment() const;
			mo_bool IsObject() const;
			mo_bool IsNativeObject() const;

			mo_bool IsSubClassOf(const JSClassInfo* info) const;
			mo_bool TryGetAsUInt32(mo_uint32& outResult) const;

			JSValue Get(JSStackFrame* exec, const UTF8String& propertyName) const;
			JSValue Get(JSStackFrame* exec, mo_unsigned propertyName) const;

			void Put(JSStackFrame* exec, const UTF8String& propertyName, JSValue value, mo_bool isStrictMode = mo_false);
			void Put(JSStackFrame* exec, mo_unsigned propertyName, JSValue value, mo_bool isStrictMode = mo_false);
			void PutToPrimitive(JSStackFrame* exec, const UTF8String& propertyName, JSValue value, JSPutPropertySlot& slot);

			JSObject* SynthesizePrototype(JSStackFrame* exec) const;

			JSCallInfo GetCallInfo(JSStackFrame* exec);
			JSCallInfo GetConstructInfo(JSStackFrame* exec);

			mo_inline const JSValue GetAsValue() const { return *this; }

			operator mo_bool() const;
			mo_bool operator==(const JSValue& other) const;
			mo_bool operator!=(const JSValue& other) const;

		private:
			mo_uint32 GetTag() const;
			mo_int32 GetPayload() const;

			JSValueData mData;
	};

#if defined(MO_PLATFORM_USE_JSVALUE_32)
	mo_inline JSValue::operator mo_bool() const
	{
		return (GetTag() != JSValueDataEmptyTag);
	}

	mo_inline mo_bool JSValue::operator==(const JSValue& other) const
	{
		return (mData.asInt64 == other.mData.asInt64);
	}

	mo_inline mo_bool JSValue::operator!=(const JSValue& other) const
	{
		return (mData.asInt64 != other.mData.asInt64);
	}
#elif defined(MO_PLATFORM_USE_JSVALUE_64)
	mo_inline JSValue::operator mo_bool() const
	{
		return mData.asPtr;
	}

	mo_inline mo_bool JSValue::operator==(const JSValue& other) const
	{
		return (mData.asPtr == other.mData.asPtr);
	}

	mo_inline mo_bool JSValue::operator!=(const JSValue& other) const
	{
		return (mData.asPtr != other.mData.asPtr);
	}
#endif
	mo_inline mo_uint32 JSValue::GetTag() const
	{
		return mData.asBits.tag;
	}

	mo_inline mo_int32 JSValue::GetPayload() const
	{
		return mData.asBits.payload;
	}


    mo_inline mo_bool operator==(const JSValue a, const JSObject* b) { return a == JSValue(b); }
    mo_inline mo_bool operator==(const JSObject* a, const JSValue b) { return JSValue(a) == b; }
    mo_inline mo_bool operator!=(const JSValue a, const JSObject* b) { return a != JSValue(b); }
    mo_inline mo_bool operator!=(const JSObject* a, const JSValue b) { return JSValue(a) != b; }

	//
	// JSValue HELPER FUNCTIONS
	//
	mo_inline_always JSValue jsEmpty()
	{
		return JSValue::MakeEmpty();
	}

	mo_inline_always JSValue jsNull()
	{
		return JSValue::MakeNull();
	}

	mo_inline_always JSValue jsUndef()
	{
		return JSValue::MakeUndefined();
	}

	mo_inline_always JSValue jsObject(JSObject* v)
	{
		return JSValue(v);
	}

	mo_inline_always JSValue jsObject(const JSObject* v)
	{
		return JSValue(v);
	}

	mo_inline_always JSValue jsBool(mo_bool v)
	{
		return JSValue::MakeBoolean(v);
	}

	mo_inline_always JSValue jsTrue()
	{
		return jsBool(mo_true);
	}

	mo_inline_always JSValue jsFalse()
	{
		return jsBool(mo_false);
	}

	mo_inline_always JSValue jsDouble(mo_double v)
	{
		return JSValue::MakeDouble(v);
	}

	mo_inline_always JSValue jsNumber(mo_double v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_int8 v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_uint8 v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_int16 v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_uint16 v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_int32 v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_uint32 v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_int64 v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_uint64 v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_long v)
	{
		return JSValue::MakeNumber(v);
	}

	mo_inline_always JSValue jsNumber(mo_ulong v)
	{
		return JSValue::MakeNumber(v);
	}
}

#endif