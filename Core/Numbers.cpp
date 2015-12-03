#include "Core/Numbers.h"
#include "double-conversion/ieee.h"
#include "double-conversion/utils.h"

namespace MoEnjin
{
	const mo_uint32 DOUBLE_STRING_BUFFER_SIZE = 96;

	const double_conversion::StringToDoubleConverter& Double::ECMAStringToDoubleConverter() {
		mo_int32 flags = double_conversion::StringToDoubleConverter::ALLOW_HEX |
						 double_conversion::StringToDoubleConverter::ALLOW_OCTALS |
						 double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES |
						 double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES;

		static double_conversion::StringToDoubleConverter converter(flags, 0.0, double_conversion::Double::NaN(), "Infinity", "NaN");
		return converter;
	}

	const double_conversion::DoubleToStringConverter& Double::ECMADoubleToStringConverter() {
		mo_int32 flags = double_conversion::DoubleToStringConverter::UNIQUE_ZERO | double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN;
		static double_conversion::DoubleToStringConverter converter(flags, "Infinity", "NaN", 'e', -6, 21, 6, 0);
		
		return converter;
	}

	mo_utf8string Double::ToString(mo_double value)
	{
		mo_utf8char buffer[DOUBLE_STRING_BUFFER_SIZE];
		double_conversion::StringBuilder builder(buffer, DOUBLE_STRING_BUFFER_SIZE);
		const DoubleToStringConverter& converter = ECMADoubleToStringConverter();
		converter.ToShortest(value, &builder);

		return mo_utf8string(builder.Finalize());
	}
}