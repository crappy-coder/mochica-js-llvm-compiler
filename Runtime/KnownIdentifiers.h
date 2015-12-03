#ifndef MOENJIN_RUNTIME_KNOWNIDENTIFIERS_H
#define MOENJIN_RUNTIME_KNOWNIDENTIFIERS_H

#include "Platform/Configuration.h"
#include "Runtime/KnownIdentifierNames.h"

namespace MoEnjin
{
	class KnownIdentifiers
	{ 
		MO_STATIC_CLASS(KnownIdentifiers);

		public:
			static const UTF8String Null;
			static const UTF8String Empty;
			static const UTF8String UnderscoreProto;
			static const UTF8String This;
			static const UTF8String UseStrict;

#define IDENTIFIER_DECLARE_KEYWORD_NAME_GLOBAL(name) static const UTF8String name##Keyword;
        KNOWN_IDENTIFIERS_EACH_KEYWORD(IDENTIFIER_DECLARE_KEYWORD_NAME_GLOBAL)
#undef IDENTIFIER_DECLARE_KEYWORD_NAME_GLOBAL
        
#define IDENTIFIER_DECLARE_PROPERTY_NAME_GLOBAL(name) static const UTF8String name;
        KNOWN_IDENTIFIERS_EACH_PROPERTY_NAME(IDENTIFIER_DECLARE_PROPERTY_NAME_GLOBAL)
#undef IDENTIFIER_DECLARE_PROPERTY_NAME_GLOBAL
	};
}

#endif