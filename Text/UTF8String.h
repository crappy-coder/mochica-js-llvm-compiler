#ifndef MOENJIN_TEXT_UTF8STRING_H
#define MOENJIN_TEXT_UTF8STRING_H

#include "Platform/Configuration.h"
#include <Poco/HashSet.h>

namespace MoEnjin
{
	class UTF8String
	{
		public:
			struct Hasher
			{
				mo_uint32 operator () (const UTF8String& value) const { return Hash::HashString(value.GetChars(), value.GetLength()); }
				mo_uint32 operator () (const UTF8String* value) const { return Hash::HashString(value->GetChars(), value->GetLength()); }
			};

		public:
			static UTF8String Make(mo_unsigned length);
			static UTF8String MakeNull();
			static UTF8String MakeEmpty();

			static UTF8String FromChar(mo_byte value);
			static UTF8String FromNumber(mo_int32 value);
			static UTF8String FromNumber(mo_int64 value);
			static UTF8String FromNumber(mo_uint64 value);
			static UTF8String FromNumber(mo_uint32 value);
			static UTF8String FromNumber(mo_double value);
			static UTF8String FromKnownString(const mo_utf8stringptr str, mo_uint32 length, mo_int32 hash = -1);
			static UTF8String FromUTF16(const mo_utf16stringptr str);

			static UTF8String Append(const UTF8String& a, const UTF8String& b);

		public:
			UTF8String();
			UTF8String(const mo_utf8string &str);
			UTF8String(const mo_utf8stringptr chars);
			UTF8String(const mo_utf8stringptr chars, mo_uint32 length);

			~UTF8String() { }

			const mo_bool IsNull() const { return mIsNull; }
			const mo_bool IsEmpty() const { return (mIsNull || mImpl.empty()); }

			const mo_uint32 GetLength() const { return (mIsNull ? 0 : mImpl.size()); }

			const mo_utf8string& Get() const { return mImpl; }
			mo_utf8string GetString() const { return mImpl; }

			const mo_utf8stringptr GetChars() const { return (mIsNull ? mo_null : mImpl.data()); }

			const mo_uint32 GetHash() const;

			mo_int32 IndexOf(mo_utf8char ch) const;
			mo_int32 IndexOf(mo_utf8char ch, mo_uint32 start) const;

			mo_int32 IndexOf(const UTF8String& str) const;
			mo_int32 IndexOf(const UTF8String& str, mo_uint32 start) const;

			UTF8String Substring(mo_uint32 start, mo_uint32 length = UINT_MAX) const;

			void Append(const UTF8String& str);
			void Insert(const UTF8String& str, mo_uint32 index);

			void Swap(UTF8String &other);
			void Clear(mo_bool nullify = mo_false);

			const mo_bool ToInt32(mo_int32& result) const;
			const mo_bool ToUInt32(mo_uint32& result) const;
			const mo_bool ToDouble(mo_double& result) const;
			const mo_bool ToECMAArrayIndex(mo_unsigned& result) const;

			mo_utf8char operator[](mo_uint32 index) const;

			UTF8String& operator=(const UTF8String& other);
			UTF8String& operator=(const mo_utf8string& other);
			UTF8String& operator=(const mo_utf8stringptr other);
			UTF8String& operator=(const mo_utf16string& other);
			UTF8String& operator=(const mo_utf16stringptr other);

			UTF8String& operator+=(const UTF8String& other);
			UTF8String& operator+=(const mo_utf8string& other);
			UTF8String& operator+=(const mo_utf8stringptr other);
			UTF8String& operator+=(const mo_utf16string& other);
			UTF8String& operator+=(const mo_utf16stringptr other);
			UTF8String& operator+=(mo_utf8char other);
			UTF8String& operator+=(mo_int32 other);
			UTF8String& operator+=(mo_int64 other);
			UTF8String& operator+=(mo_uint32 other);
			UTF8String& operator+=(mo_uint64 other);
			UTF8String& operator+=(mo_double other);

			friend UTF8String operator+(const UTF8String& a, const UTF8String& b);
			friend UTF8String operator+(const UTF8String& a, const mo_utf8string& b);
			friend UTF8String operator+(const UTF8String& a, const mo_utf8stringptr b);
			friend UTF8String operator+(const UTF8String& a, mo_utf8char b);
			friend UTF8String operator+(const UTF8String& a, mo_int32 b);
			friend UTF8String operator+(const UTF8String& a, mo_int64 b);
			friend UTF8String operator+(const UTF8String& a, mo_uint32 b);
			friend UTF8String operator+(const UTF8String& a, mo_uint64 b);
			friend UTF8String operator+(const UTF8String& a, mo_double b);

			friend mo_bool operator<(const UTF8String& a, const UTF8String& b);
			friend mo_bool operator>(const UTF8String& a, const UTF8String& b);

			friend mo_bool operator==(const UTF8String& a, const UTF8String& b);
			friend mo_bool operator==(const UTF8String& a, const mo_utf8string& b);
			friend mo_bool operator==(const UTF8String& a, const mo_utf8stringptr b);

			friend mo_bool operator!=(const UTF8String& a, const UTF8String& b);
			friend mo_bool operator!=(const UTF8String& a, const mo_utf8string& b);
			friend mo_bool operator!=(const UTF8String& a, const mo_utf8stringptr b);

			friend mo_bool operator==(const mo_utf8string& a, const UTF8String& b);
			friend mo_bool operator==(const mo_utf8stringptr a, const UTF8String& b);

			friend mo_bool operator!=(const mo_utf8string& a, const UTF8String& b);
			friend mo_bool operator!=(const mo_utf8stringptr a, const UTF8String& b);

		private:
			void ResetHash()
			{
				mHasHash = mo_false;
				mHash = 0;
			}

			mo_utf8string mImpl;
			mo_bool mIsNull;
			mutable mo_bool mHasHash;
			mutable mo_uint32 mHash;

		private:
			static mo_bool AreEqual(const UTF8String& a, const UTF8String& b);
			static mo_utf8string UTF16CharsToUTF8String(const mo_utf16stringptr source);
			static mo_utf16string UTF8CharsToUTF16String(const mo_utf8stringptr source);
	};

	typedef Poco::HashSet<UTF8String, UTF8String::Hasher> IdentifierSet;
	typedef std::vector<UTF8String> UTF8StringCollection;

	mo_inline void UTF8String::Swap(UTF8String &other)
	{
		mImpl.swap(other.mImpl);

		mIsNull = (mImpl.c_str() == mo_null);
		
		ResetHash();
	}

	mo_inline mo_int32 UTF8String::IndexOf(mo_utf8char ch) const
	{
		return IndexOf(ch, 0);
	}

	mo_inline mo_int32 UTF8String::IndexOf(mo_utf8char ch, mo_uint32 start) const
	{
		return (mIsNull ? mo_not_found : mImpl.find(ch, start));
	}

	mo_inline mo_int32 UTF8String::IndexOf(const UTF8String& str) const
	{
		return (mIsNull ? mo_not_found : mImpl.find(str.mImpl));
	}

	mo_inline mo_int32 UTF8String::IndexOf(const UTF8String& str, mo_uint32 start) const
	{
		return (mIsNull ? mo_not_found : mImpl.find(str.mImpl, start));
	}

	mo_inline mo_utf8char UTF8String::operator[] (mo_uint32 index) const
	{
		if(mIsNull || index >= mImpl.length())
			return 0;

		return mImpl[index];
	}

	mo_inline UTF8String& UTF8String::operator=(const UTF8String& other)
	{
		mImpl = other.mImpl;
		mIsNull = other.mIsNull;
		mHasHash = other.mHasHash;
		mHash = other.mHash;

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator=(const mo_utf8string& other)
	{
		mImpl = other;
		mIsNull = (other.c_str() == mo_null);
		
		ResetHash();

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator=(const mo_utf8stringptr other)
	{
		mImpl = other;
		mIsNull = (other == mo_null);
		
		ResetHash();

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator=(const mo_utf16string& other)
	{
		mImpl = UTF8String::UTF16CharsToUTF8String(other.data());
		mIsNull = (other.c_str() == mo_null);
		
		ResetHash();

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator=(const mo_utf16stringptr other)
	{
		mImpl = UTF8String::UTF16CharsToUTF8String(other);
		mIsNull = (other == mo_null);
		
		ResetHash();

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(const UTF8String& other)
	{
		if(!other.IsNull())
		{
			mImpl.append(other.mImpl);
			mIsNull = mo_false;

			ResetHash();
		}

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(const mo_utf8string& other)
	{
		if(!other.empty())
		{
			mImpl.append(other);
			mIsNull = mo_false;

			ResetHash();
		}

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(const mo_utf8stringptr other)
	{
		if(other != mo_null)
		{
			mImpl.append(other);
			mIsNull = mo_false;

			ResetHash();
		}

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(const mo_utf16string& other)
	{
		if(!other.empty())
		{
			mImpl.append(UTF8String::UTF16CharsToUTF8String(other.data()));
			mIsNull = mo_false;
		
			ResetHash();
		}

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(const mo_utf16stringptr other)
	{
		if(other != mo_null)
		{
			mImpl.append(UTF8String::UTF16CharsToUTF8String(other));
			mIsNull = mo_false;
		
			ResetHash();
		}

		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(mo_utf8char other)
	{
		*this += mo_utf8string(1, other);
		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(mo_int32 other)
	{
		*this += UTF8String::FromNumber(other);
		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(mo_int64 other)
	{
		*this += UTF8String::FromNumber(other);
		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(mo_uint32 other)
	{
		*this += UTF8String::FromNumber(other);
		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(mo_uint64 other)
	{
		*this += UTF8String::FromNumber(other);
		return *this;
	}

	mo_inline UTF8String& UTF8String::operator+=(mo_double other)
	{
		*this += UTF8String::FromNumber(other);
		return *this;
	}

	mo_inline UTF8String operator+(const UTF8String& a, const UTF8String& b)
	{
		return UTF8String::Append(a, b);
	}

	mo_inline UTF8String operator+(const UTF8String& a, const mo_utf8string& b)
	{
		return UTF8String::Append(a, b);
	}

	mo_inline UTF8String operator+(const UTF8String& a, const mo_utf8stringptr b)
	{
		return UTF8String::Append(a, b);
	}

	mo_inline UTF8String operator+(const UTF8String& a, mo_utf8char b)
	{
		return UTF8String::Append(a, UTF8String(mo_utf8string(1, b)));
	}

	mo_inline UTF8String operator+(const UTF8String& a, mo_int32 b)
	{
		return UTF8String::Append(a, UTF8String::FromNumber(b));
	}

	mo_inline UTF8String operator+(const UTF8String& a, mo_int64 b)
	{
		return UTF8String::Append(a, UTF8String::FromNumber(b));
	}

	mo_inline UTF8String operator+(const UTF8String& a, mo_uint32 b)
	{
		return UTF8String::Append(a, UTF8String::FromNumber(b));
	}

	mo_inline UTF8String operator+(const UTF8String& a, mo_uint64 b)
	{
		return UTF8String::Append(a, UTF8String::FromNumber(b));
	}

	mo_inline UTF8String operator+(const UTF8String& a, mo_double b)
	{
		return UTF8String::Append(a, UTF8String::FromNumber(b));
	}

	mo_inline mo_bool operator==(const UTF8String& a, const UTF8String& b)
	{
		mo_uint32 aLen = a.GetLength();
		mo_uint32 bLen = b.GetLength();

		if(aLen != bLen)
			return mo_false;

		if(aLen == 0)
			return mo_true;

		if(aLen == 1)
			return (a.GetChars()[0] == b.GetChars()[0]);

		return UTF8String::AreEqual(a, b);
	}

	mo_inline mo_bool operator==(const mo_utf8string& a, const UTF8String& b)
	{
		return operator==(b, a);
	}

	mo_inline mo_bool operator==(const mo_utf8stringptr a, const UTF8String& b)
	{
		return operator==(b, a);
	}

	mo_inline mo_bool operator!=(const UTF8String& a, const UTF8String& b)
	{
		return !operator==(a, b);
	}

	mo_inline mo_bool operator!=(const UTF8String& a, const mo_utf8string& b)
	{
		return !operator==(a, b);
	}

	mo_inline mo_bool operator!=(const UTF8String& a, const mo_utf8stringptr b)
	{
		return !operator==(a, b);
	}

	mo_inline mo_bool operator!=(const mo_utf8string& a, const UTF8String& b)
	{
		return !operator==(a, b);
	}

	mo_inline mo_bool operator!=(const mo_utf8stringptr a, const UTF8String& b)
	{
		return !operator==(a, b);
	}
}

#endif