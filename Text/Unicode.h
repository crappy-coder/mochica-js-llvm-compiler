#ifndef MOENJIN_TEXT_UNICODE_H
#define MOENJIN_TEXT_UNICODE_H

#include "Platform/Configuration.h"
#include <Poco/Unicode.h>

namespace MoEnjin
{
	enum CharCategory {
		NoCategory =  0,
		Other_NotAssigned =			Poco::Unicode::UCP_UNASSIGNED,
		Letter_Uppercase =			Poco::Unicode::UCP_UPPER_CASE_LETTER,
		Letter_Lowercase =			Poco::Unicode::UCP_LOWER_CASE_LETTER,
		Letter_Titlecase =			Poco::Unicode::UCP_TITLE_CASE_LETTER,
		Letter_Modifier =			Poco::Unicode::UCP_MODIFIER_LETTER,
		Letter_Other =				Poco::Unicode::UCP_OTHER_LETTER,
		Mark_NonSpacing =			Poco::Unicode::UCP_NON_SPACING_MARK,
		Mark_Enclosing =			Poco::Unicode::UCP_ENCLOSING_MARK,
		Mark_SpacingCombining =		Poco::Unicode::UCP_SPACING_MARK,
		Number_DecimalDigit =		Poco::Unicode::UCP_DECIMAL_NUMBER,
		Number_Letter =				Poco::Unicode::UCP_LETTER_NUMBER,
		Number_Other =				Poco::Unicode::UCP_OTHER_NUMBER,
		Separator_Space =			Poco::Unicode::UCP_SPACE_SEPARATOR,
		Separator_Line =			Poco::Unicode::UCP_LINE_SEPARATOR,
		Separator_Paragraph =		Poco::Unicode::UCP_PARAGRAPH_SEPARATOR,
		Other_Control =				Poco::Unicode::UCP_CONTROL,
		Other_Format =				Poco::Unicode::UCP_FORMAT,
		Other_PrivateUse =			Poco::Unicode::UCP_PRIVATE_USE,
		Other_Surrogate =			Poco::Unicode::UCP_SURROGATE,
		Punctuation_Dash =			Poco::Unicode::UCP_DASH_PUNCTUATION,
		Punctuation_Open =			Poco::Unicode::UCP_OPEN_PUNCTUATION,
		Punctuation_Close =			Poco::Unicode::UCP_CLOSE_PUNCTUATION,
		Punctuation_Connector =		Poco::Unicode::UCP_CONNECTOR_PUNCTUATION,
		Punctuation_Other =			Poco::Unicode::UCP_OTHER_PUNCTUATION,
		Symbol_Math =				Poco::Unicode::UCP_MATHEMATICAL_SYMBOL,
		Symbol_Currency =			Poco::Unicode::UCP_CURRENCY_SYMBOL,
		Symbol_Modifier =			Poco::Unicode::UCP_MODIFIER_SYMBOL,
		Symbol_Other =				Poco::Unicode::UCP_OTHER_SYMBOL,
		Punctuation_InitialQuote =	Poco::Unicode::UCP_INITIAL_PUNCTUATION,
		Punctuation_FinalQuote =	Poco::Unicode::UCP_FINAL_PUNCTUATION
	};

	CharCategory GetUnicodeCategory(mo_uint32);
}

#endif