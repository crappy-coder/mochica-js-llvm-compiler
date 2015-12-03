#ifndef MOENJIN_RUNTIME_JSSMALLSTRINGS_H
#define MOENJIN_RUNTIME_JSSMALLSTRINGS_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSString;

#define JS_FOR_EACH_COMMON_STRING_NAME(macro) \
	macro(boolean, Boolean) \
	macro(function, Function) \
	macro(string, String) \
	macro(number, Number) \
	macro(object, Object) \
	macro(undefined, Undefined) \
	macro(null, Null) \
	macro(true, True) \
	macro(false, False)

#define JS_CHARACTER_STRINGS_LENGTH (MaxCharString + 1)

	class JSSmallStrings
	{
		MO_NON_COPYABLE_CLASS(JSSmallStrings);

		public:
			static const mo_unsigned MaxCharString = 0xFF;

		public:
			JSSmallStrings();

			JSString* EmptyString(JSGlobalObject* globalObject) const
			{
				EnsureEmptyString(globalObject);
				return mEmptyString;
			}

			JSString* CharacterString(JSGlobalObject* globalObject, mo_byte ch) const
			{
				EnsureCharacterString(globalObject, ch);
				return mCharStrings[ch];
			}

		private:
			void EnsureEmptyString(JSGlobalObject* globalObject) const;
			void EnsureCharacterString(JSGlobalObject* globalObject, mo_byte ch) const;
			void InitializeKeyword(JSGlobalObject* globalObject, JSString*& str, const mo_utf8stringptr value) const;

			mutable JSString* mEmptyString;
			mutable JSString* mCharStrings[JS_CHARACTER_STRINGS_LENGTH];

		public:
#define DEFINE_COMMON_STRING_ACCESSOR(lname,uname) \
			JSString* Get##uname##Keyword(JSGlobalObject* globalObject) const \
			{ \
				if(!m##uname##Keyword) \
					InitializeKeyword(globalObject, m##uname##Keyword, #lname); \
				\
				return m##uname##Keyword; \
			}
		JS_FOR_EACH_COMMON_STRING_NAME(DEFINE_COMMON_STRING_ACCESSOR)
#undef DEFINE_COMMON_STRING_ACCESSOR

		private:

#define DEFINE_COMMON_STRING_STORAGE(lname,uname) mutable JSString* m##uname##Keyword;
		JS_FOR_EACH_COMMON_STRING_NAME(DEFINE_COMMON_STRING_STORAGE)
#undef DEFINE_COMMON_STRING_STORAGE
	};
}

#endif