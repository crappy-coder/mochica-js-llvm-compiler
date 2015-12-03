#ifndef MOENJIN_TEXT_TEXT_H
#define MOENJIN_TEXT_TEXT_H

#include "Platform/Configuration.h"
#include "Text/ASCII.h"

namespace MoEnjin
{
	// TODO - need to add in support for BOM (0xFEFF) and Unicode space separator "Zs" characters.
	mo_inline bool IsWhiteSpace(mo_utf8char ch)
	{
		return (ch == ' ' || ch == '\t' || ch == 0xB || ch == 0xC || ch == 0xA0);
	}

	// TODO - need to add in support for line and paragraph separators (0x2028 and 0x2029)
	mo_inline bool IsLineTerminator(mo_utf8char ch)
	{
		return (ch == '\r' || ch == '\n');
	}

	mo_inline mo_byte ToHex(int a, int b)
	{
		return (ToASCIIHexValue(a) << 4) | ToASCIIHexValue(b);
	}

	mo_inline mo_utf16char ToUnicode(int a, int b, int c, int d)
	{
		return ((ToHex(a, b) << 8) | ToHex(c, d));
	}

	mo_inline mo_utf8char SingleCharacterEscape(mo_utf8char ch)
	{
		switch(ch)
		{
			case 'b':
				return 0x08;
			case 't':
				return 0x09;
			case 'n':
				return 0x0A;
			case 'v':
				return 0x0B;
			case 'f':
				return 0x0C;
			case 'r':
				return 0x0D;
			case '\\':
				return '\\';
			case '\'':
				return '\'';
			case '"':
				return '"';
			default:
				return 0;
		}
	}

	static mo_int32 ParseDigit(mo_uint16 ch, mo_int32 radix)
	{
		mo_int32 digit = -1;

		// [0-9]
		if (ch >= '0' && ch <= '9')
			digit = ch - '0';

		// [A-Z]
		else if (ch >= 'A' && ch <= 'Z')
			digit = ch - 'A' + 10;

		// [a-z]
		else if (ch >= 'a' && ch <= 'z')
			digit = ch - 'a' + 10;

		if (digit >= radix)
			return -1;

		return digit;
	}

	static mo_double ParseIntOverflow(const mo_utf8stringptr str, mo_int32 length, mo_int32 radix)
	{
		mo_double number = 0.0;
		mo_double radixMultiplier = 1.0;
		
		for (const mo_utf8stringptr p = str + length - 1; p >= str; p--) 
		{
			if (radixMultiplier == std::numeric_limits<mo_double>::infinity()) 
			{
				if (*p != '0') 
				{
					number = std::numeric_limits<mo_double>::infinity();
					break;
				}
			} 
			else 
			{
				mo_int32 digit = ParseDigit(*p, radix);

				number += digit * radixMultiplier;
			}

			radixMultiplier *= radix;
		}

		return number;
	}
}

#endif