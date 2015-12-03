#include "Runtime/JSConvert.h"
#include "Runtime/JSConstructorFunction.h"
#include "Runtime/JSFunction.h"
#include "Runtime/JSGetterSetter.h"
#include "Runtime/JSObject.h"
#include "Core/Numbers.h"
#include "Text/ASCII.h"
#include "Text/Text.h"

namespace MoEnjin
{
	const mo_double JSConvert::ToNumber(const UTF8String& str)
	{
		mo_unsigned len = str.GetLength();

		if(len == 1)
		{
			mo_utf8char ch = str[0];

			if(IsASCIIDigit(ch))
				return (ch - '0');

			if(IsWhiteSpace(ch))
				return 0;

			return std::numeric_limits<mo_double>::quiet_NaN();
		}
		
		mo_uint32 n = 0;

		return Double::Parse(str.GetChars(), len, n);
	}

	const mo_uint32 JSConvert::ToUInt32(const mo_double value)
	{
		return JSConvert::ToInt32(value);
	}

	// this ToInt32 operation is defined in section 9.5 of the ECMA-262 spec.
	//
	// the operation can be described as round towards zero, then select the 32 least
	// bits of the resulting value in 2's-complement representation.
	//
	const mo_int32 JSConvert::ToInt32(const mo_double value)
	{
		mo_int64 bits = bitwise_cast<mo_int64>(value);
		mo_int32 exp = (MO_CAST(mo_int32, (bits >> 52)) & 0x7ff) - 0x3ff;

		// if exponent < 0 there will be no bits to the left of the decimal point
		// after rounding; if the exponent is > 83 then no bits of precision can be
		// left in the low 32-bit range of the result (IEEE-754 doubles have 52 bits
		// of fractional precision).
		//
		// note this case handles 0, -0, and all infinite, NaN, & denormal value. 
		if(exp < 0 || exp > 83)
			return 0;

		// select the appropriate 32-bits from the floating point mantissa.  If the
		// exponent is 52 then the bits we need to select are already aligned to the
		// lowest bits of the 64-bit integer representation of the number, no need
		// to shift.  If the exponent is greater than 52 we need to shift the value
		// left by (exp - 52), if the value is less than 52 we need to shift right
		// accordingly.
		mo_int32 result = (exp > 52) 
			? MO_CAST(mo_int32, (bits << (exp - 52))) 
			: MO_CAST(mo_int32, (bits >> (52 - exp)));

		// IEEE-754 double precision values are stored omitting an implicit 1 before
		// the decimal point; we need to reinsert this now.  We may also the shifted
		// invalid bits into the result that are not a part of the mantissa (the sign
		// and exponent bits from the floating point representation); mask these out.
		if(exp < 32)
		{
			mo_int32 missingOne = 1 << exp;
			result &= missingOne - 1;
			result += missingOne;
		}

		// If the input value was negative (we could test either 'number' or 'bits',
		// but testing 'bits' is likely faster) invert the result appropriately.
		return (bits < 0 ? -result : result);
	}

	JSConstructorFunction* JSConvert::ToConstructorFunction(JSValue value)
	{
		JSObject* obj = ToNativeObject(value);
		MO_ASSERT(obj->IsSubClassOf(&JSConstructorFunction::ClassInfo));

		return mo_cast<JSConstructorFunction*>(obj);
	}

	JSFunction* JSConvert::ToFunction(JSValue value)
	{
		if(value.IsNativeObject() && (value.GetAsNativeObject()->GetType() == JSObjectTypeFunction))
			return mo_cast<JSFunction*>(value.GetAsNativeObject());

		return mo_null;
	}

	JSGetterSetter* JSConvert::ToGetterSetter(JSValue value)
	{
		MO_ASSERT(value.GetAsNativeObject()->IsGetterOrSetter());

		return mo_cast<JSGetterSetter*>(value.GetAsNativeObject());
	}

	JSObject* JSConvert::ToNativeObject(JSValue value)
	{
		MO_ASSERT(value.IsNativeObject());

		return value.GetAsNativeObject();
	}
}