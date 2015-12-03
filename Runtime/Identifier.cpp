#include "Identifier.h"

namespace MoEnjin
{
	Identifier::Identifier() { }

	Identifier::Identifier(const UTF8String& value)
		: mImpl(value) { }

	Identifier::Identifier(const mo_utf8stringptr value)
		: mImpl(value) { }

	Identifier Identifier::From(mo_unsigned value)
	{
		return Identifier(UTF8String::FromNumber(value));
	}

	Identifier Identifier::From(mo_int64 value)
	{
		return Identifier(UTF8String::FromNumber(value));
	}

	Identifier Identifier::From(mo_double value)
	{
		return Identifier(UTF8String::FromNumber(value));
	}

	mo_bool Identifier::AreEqual(const UTF8String& a, const UTF8String& b)
	{
		return (a.mImpl == b.mImpl);
	}

	mo_bool Identifier::AreEqual(const UTF8String& a, const UTF8String& b)
	{
		return (a.mImpl == b);
	}

	mo_bool Identifier::AreEqual(const UTF8String& a, const mo_utf8stringptr b)
	{
		return (a.mImpl == b);
	}
}