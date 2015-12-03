#ifndef MOENJIN_CORE_NUMBERS_H
#define MOENJIN_CORE_NUMBERS_H

#include "Platform/Configuration.h"
#include "Text/Text.h"

#include "double-conversion/double-conversion.h"

namespace MoEnjin
{
	typedef double_conversion::StringToDoubleConverter StringToDoubleConverter;
	typedef double_conversion::DoubleToStringConverter DoubleToStringConverter;
	
	class Double
	{
		MO_STATIC_CLASS(Double);

		static const StringToDoubleConverter& ECMAStringToDoubleConverter();
		static const DoubleToStringConverter& ECMADoubleToStringConverter();

		public:
			static mo_double Parse(const mo_utf8stringptr chars, mo_uint32 length, mo_uint32& outNumChars);
			static mo_utf8string ToString(mo_double value);
	};

	mo_inline mo_double Double::Parse(const mo_utf8stringptr chars, mo_uint32 length, mo_uint32& outNumChars)
	{
		const StringToDoubleConverter& converter = ECMAStringToDoubleConverter();

		int n = 0;
		mo_double r = converter.StringToDouble(chars, length, &n);

		outNumChars = n;
		return r;
	}
}

#endif