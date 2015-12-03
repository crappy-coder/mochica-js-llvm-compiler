#include "Text/UTF8String.h"
#include "Core/Numbers.h"

namespace MoEnjin
{
	UTF8String::UTF8String()
		: mIsNull(mo_false)
		, mHasHash(mo_false)
		, mHash(0) { }

	UTF8String::UTF8String(const mo_utf8string &str)
		: mImpl(str)
		, mIsNull(false)
		, mHasHash(mo_false)
		, mHash(0) { }

	UTF8String::UTF8String(const mo_utf8stringptr chars)
		: mImpl(chars == mo_null ? "" : chars)
		, mIsNull(chars == mo_null)
		, mHasHash(mo_false)
		, mHash(0) { }

	UTF8String::UTF8String(const mo_utf8stringptr chars, mo_uint32 length)
		: mImpl(chars == mo_null ? "" : chars, length)
		, mIsNull(chars == mo_null)
		, mHasHash(mo_false)
		, mHash(0) { }

	const mo_uint32 UTF8String::GetHash() const
	{
		if(mHasHash)
			return mHash;

		mHash = Hash::HashString(mImpl.data(), mImpl.length());
		mHasHash = mo_true;
		return mHash;
	}

	UTF8String UTF8String::Substring(mo_uint32 start, mo_uint32 length) const
	{
		mo_uint32 len = GetLength();

		start = MO_MIN(start, len);
		length = MO_MIN(length, len - start);

		// asking for the full string
		if(start == 0 && length == len)
			return *this;

		return UTF8String(mo_utf8string(mImpl, start, length));
	}

	void UTF8String::Append(const UTF8String& str)
	{
		mImpl.append(str.Get());
		mIsNull = mo_false;
	}

	void UTF8String::Insert(const UTF8String& str, mo_uint32 index)
	{
		mImpl.insert(index, str.mImpl);
		mIsNull = mo_false;
	}

	void UTF8String::Clear(mo_bool nullify)
	{
		mImpl.clear();
		mIsNull = nullify;
		mHasHash = mo_false;
		mHash = 0;
	}

	const mo_bool UTF8String::ToInt32(mo_int32& result) const
	{
		char t;
		return (std::sscanf(mImpl.c_str(), "%d%c", &result, &t) == 1);
	}

	const mo_bool UTF8String::ToUInt32(mo_uint32& result) const
	{
		result = 0;

		// empty strings are not considered a valid number
		if(IsEmpty())
			return mo_false;

		const mo_utf8stringptr chars = mImpl.c_str();
		mo_uint32 value = chars[0] - '0';
		mo_uint32 len = mImpl.length();

		if(value > 9)
			return mo_false;

		// the first character is zero, which means if the length is greater 
		// than 1 it won't be a valid number (i.e. 012 is invalid)
		if(!value && len > 1)
			return mo_false;

		while(--len)
		{
			// check for 32 bit overflow
			if(value > 0xFFFFFFFFU / 10)
				return mo_false;

			value *= 10;

			// get next character and conver to a digit
			mo_uint32 nextValue = *(++chars) - '0';

			if(nextValue > 9)
				return mo_false;

			// add previous value
			nextValue += value;

			// check for 32 bit overflow
			if(nextValue < value)
				return mo_false;

			value = nextValue;
		}

		result = value;
		return mo_true;
	}

	const mo_bool UTF8String::ToDouble(mo_double& result) const
	{
		mo_uint32 n = 0;
		result = Double::Parse(mImpl.c_str(), mImpl.length(), n);
		return mo_true;
	}

	const mo_bool UTF8String::ToECMAArrayIndex(mo_unsigned& result) const
	{
		if(ToUInt32(result) && result >= 0xFFFFFFFFU)
			return mo_false;

		return mo_true;
	}

	mo_bool operator<(const UTF8String& a, const UTF8String& b)
	{
		const mo_utf8stringptr aCh = a.GetChars();
		const mo_utf8stringptr bCh = b.GetChars();
		const mo_uint32 aLen = a.GetLength();
		const mo_uint32 bLen = b.GetLength();
		const mo_uint32 minLen = MO_MIN(aLen, bLen);
		mo_uint32 len = 0;

		while(len < minLen && *aCh == *bCh)
		{
			aCh++; 
			bCh++;
			len++;
		}

		if(len < minLen)
			return (aCh[0] < bCh[0]);

		return (aLen < bLen);
	}

	mo_bool operator>(const UTF8String& a, const UTF8String& b)
	{
		const mo_utf8stringptr aCh = a.GetChars();
		const mo_utf8stringptr bCh = b.GetChars();
		const mo_uint32 aLen = a.GetLength();
		const mo_uint32 bLen = b.GetLength();
		const mo_uint32 minLen = MO_MIN(aLen, bLen);
		mo_uint32 len = 0;

		while(len < minLen && *aCh == *bCh)
		{
			aCh++; 
			bCh++;
			len++;
		}

		if(len < minLen)
			return (aCh[0] > bCh[0]);

		return (aLen > bLen);
	}

	mo_bool operator==(const UTF8String& a, const mo_utf8string& b)
	{
		if(a.IsEmpty())
			return b.empty();

		return UTF8String::AreEqual(a, UTF8String(b));
	}

	mo_bool operator==(const UTF8String& a, const mo_utf8stringptr b)
	{
		if(a.IsEmpty())
			return (b == mo_null);

		return UTF8String::AreEqual(a, UTF8String(b));
	}

	UTF8String UTF8String::Make(mo_unsigned length)
	{
		mo_utf8string str;
		str.reserve(length);

		return UTF8String(str);
	}

	UTF8String UTF8String::MakeNull()
	{
		return UTF8String(mo_null);
	}

	UTF8String UTF8String::MakeEmpty()
	{
		return UTF8String("");
	}

	UTF8String UTF8String::FromChar(mo_byte value)
	{
		return UTF8String(mo_cast_type<mo_utf8char*>(&value), 1);
	}

	UTF8String UTF8String::FromNumber(mo_int32 value)
	{
		mo_utf8char buf[1 + sizeof(value) * 3];
		mo_utf8char* end = buf + MO_ARRAY_LENGTH(buf);
		mo_utf8char* p = end;

		if(value == 0)
			*--p = '0';
		else if(value == INT_MIN)
		{
			mo_utf8char minBuf[1 + sizeof(value) * 3];
			snprintf(minBuf, sizeof(minBuf), "%d", INT_MIN);
			return UTF8String(minBuf);
		}
		else
		{
			mo_bool neg = mo_false;

			if(value < 0)
			{
				neg = mo_true;
				value = -value;
			}

			while(value)
			{
				*--p = (value % 10) + '0';
				value /= 10;
			}

			if(neg)
				*--p = '-';
		}

		return UTF8String(p, mo_cast<mo_uint32>(end - p));
	}

	UTF8String UTF8String::FromNumber(mo_int64 value)
	{
		mo_utf8char buf[1 + sizeof(value) * 3];
		mo_utf8char* end = buf + MO_ARRAY_LENGTH(buf);
		mo_utf8char* p = end;

		if(value == 0)
			*--p = '0';
		else if(value == std::numeric_limits<mo_int64>::min())
		{
			mo_utf8char minBuf[1 + sizeof(value) * 3];
			snprintf(minBuf, sizeof(minBuf), "%"MO_INT64_FORMAT"d", std::numeric_limits<mo_int64>::min());
			return UTF8String(minBuf);
		}
		else
		{
			mo_bool neg = mo_false;

			if(value < 0)
			{
				neg = mo_true;
				value = -value;
			}

			while(value)
			{
				*--p = (value % 10) + '0';
				value /= 10;
			}

			if(neg)
				*--p = '-';
		}

		return UTF8String(p, mo_cast<mo_uint32>(end - p));
	}

	UTF8String UTF8String::FromNumber(mo_uint64 value)
	{
		mo_utf8char buf[sizeof(value) * 3];
		mo_utf8char* end = buf + MO_ARRAY_LENGTH(buf);
		mo_utf8char* p = end;

		if(value == 0)
			*--p = '0';
		else
		{
			while(value)
			{
				*--p = (value % 10) + '0';
				value /= 10;
			}
		}

		return UTF8String(p, mo_cast<mo_uint32>(end - p));
	}

	UTF8String UTF8String::FromNumber(mo_uint32 value)
	{
		mo_utf8char buf[sizeof(value) * 3];
		mo_utf8char* end = buf + MO_ARRAY_LENGTH(buf);
		mo_utf8char* p = end;

		if(value == 0)
			*--p = '0';
		else
		{
			while(value)
			{
				*--p = (value % 10) + '0';
				value /= 10;
			}
		}

		return UTF8String(p, mo_cast<mo_uint32>(end - p));
	}

	UTF8String UTF8String::FromNumber(mo_double value)
	{
		return UTF8String(Double::ToString(value));
	}

	UTF8String UTF8String::FromKnownString(const mo_utf8stringptr str, mo_uint32 length, mo_int32 hash)
	{
		UTF8String result(str, length);

		if(hash > 0)
		{
			result.mHash = mo_cast<mo_uint32>(hash);
			result.mHasHash = mo_true;
		}

		return result;
	}

	UTF8String UTF8String::FromUTF16(const mo_utf16stringptr str)
	{
		return UTF8String(UTF16CharsToUTF8String(str));
	}

	UTF8String UTF8String::Append(const UTF8String& a, const UTF8String& b)
	{
		mo_utf8string str(a.mImpl);
		str.append(b.mImpl);

		return UTF8String(str);
	}

	mo_bool UTF8String::AreEqual(const UTF8String& a, const UTF8String& b)
	{
		// TODO - use hash for equal comparison?

		const mo_uint32 aLen = a.GetLength();
		const mo_uint32 bLen = b.GetLength();
		const mo_utf8stringptr aChars = a.GetChars();
		const mo_utf8stringptr bChars = b.GetChars();

		if(aLen != bLen)
			return mo_false;

		if(aLen == 1)
			return (aChars[0] == bChars[0]);

		if(aLen == 2)
			return ((aChars[0] == bChars[0]) && (aChars[1] == bChars[1]));

		return !memcmp(aChars, bChars, aLen * sizeof(mo_utf8char));
	}

	mo_utf8string UTF8String::UTF16CharsToUTF8String(const mo_utf16stringptr source)
	{
		if(!source)
			return "";

		std::mbstate_t state;
		size_t len = std::wcsrtombs(mo_null, &source, 0, &state);
		mo_utf8string str(len, '\0');

		std::wcsrtombs(&str.front(), &source, str.size(), &state);

		return str;
	}

	mo_utf16string UTF8String::UTF8CharsToUTF16String(const mo_utf8stringptr source)
	{
		if(!source)
			return MO_WIDE("");

		std::mbstate_t state;
		
		size_t len = std::mbsrtowcs(mo_null, &source, 0, &state);
		mo_utf16string str(len, '\0');

		std::mbsrtowcs(&str.front(), &source, str.size(), &state);

		return str;
	}
}