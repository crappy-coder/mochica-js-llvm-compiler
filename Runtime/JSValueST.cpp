#include "Runtime/JSValue.h"
#include "Runtime/JSObject.h"

namespace MoEnjin
{
#if defined(MO_PLATFORM_USE_JSVALUE_32)
	JSValueImpl JSValue::Encode(JSValue value)
	{
		return value.mData.asInt64;
	}

	JSValue JSValue::Decode(JSValueImpl encValue)
	{
		JSValue v;
		v.mData.asInt64 = encValue;
		return v;
	}
#elif defined(MO_PLATFORM_USE_JSVALUE_64)
	JSValueImpl JSValue::Encode(JSValue value)
	{
		return value.mData.asInt64;
	}

	JSValue JSValue::Decode(JSValueImpl encValue)
	{
		return JSValue(reinterpret_cast<JSNativeObject*>(encValue));
	}
#endif

	JSValue JSValue::MakeEmpty()
	{
		return JSValue();
	}

	JSValue JSValue::MakeUndefined()
	{
		return JSValue(JSValueUndefined);
	}

	JSValue JSValue::MakeNull()
	{
		return JSValue(JSValueNull);
	}

	JSValue JSValue::MakeTrue()
	{
		return JSValue(JSValueTrue);
	}

	JSValue JSValue::MakeFalse()
	{
		return JSValue(JSValueFalse);
	}

	JSValue JSValue::MakeBoolean(mo_bool value)
	{
		return (value ? MakeTrue() : MakeFalse());
	}

	JSValue JSValue::MakeDouble(mo_double value)
	{
		JSValue ret(JSValueDouble, value);
		MO_ASSERT(ret.IsNumber());
		return ret;
	}

	JSValue JSValue::MakeNumber(mo_double value)
	{
		JSValue ret(value);
		MO_ASSERT(ret.IsNumber());
		return ret;
	}

	JSValue JSValue::MakeNumber(mo_int8 value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_int16 value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_int32 value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_int64 value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_long value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_uint8 value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_uint16 value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_uint32 value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_uint64 value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeNumber(mo_ulong value)
	{
		return JSValue(value);
	}

	JSValue JSValue::MakeString(const UTF8String& value)
	{
		return JSValue(value.GetChars());
	}

	mo_bool JSValue::AreEqual(JSValue a, JSValue b)
	{
		return mo_false;
	}

	mo_bool JSValue::AreStrictEqual(JSValue a, JSValue b)
	{
		return mo_false;
	}

	mo_bool JSValue::AreSameValue(JSValue a, JSValue b)
	{
		return mo_false;
	}
}