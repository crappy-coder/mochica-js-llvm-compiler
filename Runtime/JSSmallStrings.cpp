#include "Runtime/JSSmallStrings.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSString.h"

namespace MoEnjin
{
#define INIT_COMMON_STRING(lname,uname) , m##uname##Keyword(mo_null)

	JSSmallStrings::JSSmallStrings()
		: mEmptyString(mo_null)
		JS_FOR_EACH_COMMON_STRING_NAME(INIT_COMMON_STRING)
	{
		for(mo_unsigned i = 0; i < JS_CHARACTER_STRINGS_LENGTH; ++i)
			mCharStrings[i] = mo_null;	
	}

#undef INIT_COMMON_STRING

	void JSSmallStrings::EnsureEmptyString(JSGlobalObject* globalObject) const
	{
		if(mEmptyString)
			return;

		mEmptyString = JSString::Create(globalObject, UTF8String::MakeEmpty());
	}

	void JSSmallStrings::EnsureCharacterString(JSGlobalObject* globalObject, mo_byte ch) const
	{
		if(mCharStrings[ch])
			return;

		mCharStrings[ch] = JSString::Create(globalObject, UTF8String::FromChar(ch));
	}

	void JSSmallStrings::InitializeKeyword(JSGlobalObject* globalObject, JSString*& str, const mo_utf8stringptr value) const
	{
		str = JSString::Create(globalObject, value);
	}
}