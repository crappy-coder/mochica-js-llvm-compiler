#ifndef MOENJIN_TEXT_ASCII_H
#define MOENJIN_TEXT_ASCII_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	template <typename CharType> mo_inline bool IsASCII(CharType ch)
	{
		return !(ch & ~0x7F);
	}

	template<typename CharType> mo_inline bool IsASCIIAlpha(CharType ch)
	{
		return (ch | 0x20) >= 'a' && (ch | 0x20) <= 'z';
	}

	template<typename CharType> mo_inline bool IsASCIIDigit(CharType ch)
	{
		return ch >= '0' && ch <= '9';
	}

	template<typename CharType> mo_inline bool IsASCIIAlphaNumeric(CharType ch)
	{
		return (IsASCIIDigit(ch) || IsASCIIAlpha(ch));
	}

	template<typename CharType> mo_inline bool IsASCIIHexDigit(CharType ch)
	{
		return IsASCIIDigit(ch) || ((ch | 0x20) >= 'a' && (ch | 0x20) <= 'f');
	}

	template<typename CharType> mo_inline bool IsASCIILower(CharType ch)
	{
		return (ch >= 'a' && ch <= 'z');
	}

	template<typename CharType> mo_inline bool IsASCIIUpper(CharType ch)
	{
		return ch >= 'A' && ch <= 'Z';
	}

	template<typename CharType> mo_inline bool IsASCIIOctalDigit(CharType ch)
	{
		return (ch >= '0') & (ch <= '7');
	}

	template<typename CharType> mo_inline bool IsASCIIPrintable(CharType ch)
	{
		return (ch >= ' ' && ch <= '~');
	}

	template<typename CharType> mo_inline bool IsASCIISpace(CharType ch)
	{
		return ch <= ' ' && (ch == ' ' || (ch <= 0xD && ch >= 0x9));
	}

	template<typename CharType> mo_inline CharType ToASCIILower(CharType ch)
	{
		return ch | ((ch >= 'A' && ch <= 'Z') << 5);
	}

	template<typename CharType> mo_inline CharType ToASCIIUpper(CharType ch)
	{
		return ch & ~((ch >= 'a' && ch <= 'z') << 5);
	}

	template<typename CharType> mo_inline int ToASCIIHexValue(CharType ch)
	{
		MO_ASSERT(IsASCIIHexDigit(ch));

		return ch < 'A' ? ch - '0' : (ch - 'A' + 10) & 0xF;
	}

	template<typename CharType> mo_inline int ToASCIIHexValue(CharType upperValue, CharType lowerValue)
	{
		MO_ASSERT(IsASCIIHexDigit(upperValue) && IsASCIIHexDigit(lowerValue));

		return ((ToASCIIHexValue(upperValue) << 4) & 0xF0) | ToASCIIHexValue(lowerValue);
	}

	mo_inline mo_utf8char LowerNibbleToASCIIHexDigit(mo_utf8char ch)
	{
		mo_utf8char nibble = ch & 0xF;

		return nibble < 10 ? '0' + nibble : 'A' + nibble - 10;
	}

	mo_inline mo_utf8char UpperNibbleToASCIIHexDigit(mo_utf8char ch)
	{
		mo_utf8char nibble = (ch >> 4) & 0xF;

		return nibble < 10 ? '0' + nibble : 'A' + nibble - 10;
	}
}

#endif