#include "KnownIdentifiers.h"

namespace MoEnjin
{
	#define INITIALIZE_PROPERTY_NAME(name) const UTF8String KnownIdentifiers::##name(#name);
	#define INITIALIZE_KEYWORD(name) const UTF8String KnownIdentifiers::##name##Keyword(#name);

	const UTF8String KnownIdentifiers::Null;
	const UTF8String KnownIdentifiers::Empty = "";
	const UTF8String KnownIdentifiers::UnderscoreProto = "__proto__";
	const UTF8String KnownIdentifiers::This = "this";
	const UTF8String KnownIdentifiers::UseStrict = "use strict";

	KNOWN_IDENTIFIERS_EACH_KEYWORD(INITIALIZE_KEYWORD)
	KNOWN_IDENTIFIERS_EACH_PROPERTY_NAME(INITIALIZE_PROPERTY_NAME)
}