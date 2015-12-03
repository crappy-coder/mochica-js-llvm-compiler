#ifndef MOENJIN_RUNTIME_IDENTIFIER_H
#define MOENJIN_RUNTIME_IDENTIFIER_H

#include "Platform/Configuration.h"
#include "Text/UTF8String.h"
#include <Poco/HashSet.h>

namespace MoEnjin
{
	class Identifier
	{	
		public:
			Identifier();
			Identifier(const UTF8String& value);
			Identifier(const mo_utf8stringptr value);

			//UTF8String& Get() { return mImpl; }
			const UTF8String& Get() const { return mImpl; }
			const mo_utf8stringptr GetChars() const { return mImpl.GetChars(); }
			UTF8String GetString() const { return mImpl; }
				
			const mo_uint32 GetLength() const { return mImpl.GetLength(); }
			const mo_uint32 GetHash() { return mImpl.GetHash(); }

			const mo_bool IsEmpty() const { return mImpl.IsEmpty(); }
			const mo_bool IsNull() const { return mImpl.IsNull(); }

			friend mo_bool operator==(const UTF8String& a, const UTF8String& b);
			friend mo_bool operator==(const UTF8String& a, const UTF8String& b);
			friend mo_bool operator==(const UTF8String& a, const mo_utf8stringptr b);

			friend mo_bool operator!=(const UTF8String& a, const UTF8String& b);
			friend mo_bool operator!=(const UTF8String& a, const UTF8String& b);
			friend mo_bool operator!=(const UTF8String& a, const mo_utf8stringptr b);

		private:
			UTF8String mImpl;

		public:
			static Identifier From(mo_unsigned value);
			static Identifier From(mo_int64 value);
			static Identifier From(mo_double value);

			static mo_bool AreEqual(const UTF8String& a, const UTF8String& b);
			static mo_bool AreEqual(const UTF8String& a, const UTF8String& b);
			static mo_bool AreEqual(const UTF8String& a, const mo_utf8stringptr b);
	};

	mo_inline mo_bool operator==(const UTF8String& a, const UTF8String& b)
	{
		return Identifier::AreEqual(a, b);
	}

	mo_inline mo_bool operator==(const UTF8String& a, const UTF8String& b)
	{
		return Identifier::AreEqual(a, b);
	}

	mo_inline mo_bool operator==(const UTF8String& a, const mo_utf8stringptr b)
	{
		return Identifier::AreEqual(a, b);
	}

	mo_inline mo_bool operator!=(const UTF8String& a, const UTF8String& b)
	{
		return !operator==(a, b);
	}

	mo_inline mo_bool operator!=(const UTF8String& a, const UTF8String& b)
	{
		return !operator==(a, b);
	}

	mo_inline mo_bool operator!=(const UTF8String& a, const mo_utf8stringptr b)
	{
		return !operator==(a, b);
	}

	
}

#endif