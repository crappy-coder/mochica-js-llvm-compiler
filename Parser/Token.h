#ifndef MOENJIN_PARSER_TOKEN_H
#define MOENJIN_PARSER_TOKEN_H

#include "Platform/Configuration.h"
#include "Parser/TokenType.h"

namespace MoEnjin
{
	union TokenData 
	{
		mo_int32 intValue;
		mo_double doubleValue;
		const UTF8String* id;
	};

	struct TokenIdentifierValue
	{
		const mo_utf8stringptr key;
		TokenType token;
	};

	struct Token
	{
		TokenType type;
		TokenData data;
		int line;
		int begin;
		int end;
	};

	static const int TOK_IDENTIFIER_TABLE_SIZE = 45;
	static const struct TokenIdentifierValue TOK_IDENTIFIER_TABLE[TOK_IDENTIFIER_TABLE_SIZE] = {
	   { "null",		TOK_NULL},
	   { "true",		TOK_TRUE},
	   { "false",		TOK_FALSE},
	   { "break",		TOK_BREAK},
	   { "case",		TOK_CASE},
	   { "catch",		TOK_CATCH},
	   { "const",		TOK_CONST},
	   { "default",		TOK_DEFAULT},
	   { "finally",		TOK_FINALLY},
	   { "for",			TOK_FOR},
	   { "instanceof",	TOK_INSTANCEOF},
	   { "new",			TOK_NEW},
	   { "var",			TOK_VAR},
	   { "continue",	TOK_CONTINUE},
	   { "function",	TOK_FUNCTION},
	   { "return",		TOK_RETURN},
	   { "void",		TOK_VOID},
	   { "delete",		TOK_DELETE},
	   { "if",			TOK_IF},
	   { "this",		TOK_THIS},
	   { "do",			TOK_DO},
	   { "while",		TOK_WHILE},
	   { "else",		TOK_ELSE},
	   { "in",			TOK_IN},
	   { "switch",		TOK_SWITCH},
	   { "throw",		TOK_THROW},
	   { "try",			TOK_TRY},
	   { "typeof",		TOK_TYPEOF},
	   { "with",		TOK_WITH},
	   { "debugger",	TOK_DEBUGGER},
	   { "class",		TOK_RESERVED},
	   { "enum",		TOK_RESERVED},
	   { "export",		TOK_RESERVED},
	   { "extends",		TOK_RESERVED},
	   { "import",		TOK_RESERVED},
	   { "super",		TOK_RESERVED},
	   { "implements",	TOK_RESERVED_IF_STRICT},
	   { "interface",	TOK_RESERVED_IF_STRICT},
	   { "let",			TOK_RESERVED_IF_STRICT},
	   { "package",		TOK_RESERVED_IF_STRICT},
	   { "private",		TOK_RESERVED_IF_STRICT},
	   { "protected",	TOK_RESERVED_IF_STRICT},
	   { "public",		TOK_RESERVED_IF_STRICT},
	   { "static",		TOK_RESERVED_IF_STRICT},
	   { "yield",		TOK_RESERVED_IF_STRICT},
	};

	static const TokenType GetTokenTypeFromIdentifer(const UTF8String* id)
	{
		for(mo_int32 i = 0; i < TOK_IDENTIFIER_TABLE_SIZE; ++i)
		{
			if(id->Get() == TOK_IDENTIFIER_TABLE[i].key)
				return TOK_IDENTIFIER_TABLE[i].token;
		}

		return TOK_UNKNOWN;
	}
}

#endif