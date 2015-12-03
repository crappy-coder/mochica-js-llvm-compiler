#ifndef MOENJIN_PARSER_LEXER_H
#define MOENJIN_PARSER_LEXER_H

#include "Platform/Configuration.h"
#include "Parser/Token.h"

namespace MoEnjin
{
	class GlobalContext;
	class SourceCode;
	class Lexer
	{
		MO_NON_COPYABLE_CLASS(Lexer);

		struct CodeInfo
		{
			const mo_utf8char* current;
			const mo_utf8char* start;
			const mo_utf8char* end;

			CodeInfo()
				: current(0), start(0), end(0) { }
		};
		
		public:
			Lexer(GlobalContext* globalContext, const SourceCode& source);
			~Lexer();

			const SourceCode* GetSource() const { return mSource; }
			const UTF8String GetError() const { return mError; }
			const TokenType GetLastToken() const { return mLastToken; }
			const mo_int32 GetLastLineNumber() const { return mLastLineNumber; }
			const mo_int32 GetLineNumber() const { return mLineNumber; }

			void SetSource(const SourceCode& source);
			void SetIsReparsing() { mIsReparsing = true; }
			void SetPosition(int position);
			void SetLineNumber(mo_int32 value) { mLineNumber = value; }
			void SetLastLineNumber(mo_int32 lastLine) { mLastLineNumber = lastLine; }

			const mo_bool IsNextTokenColon() const;
			const mo_bool IsReparsing() const { return mIsReparsing; }

			const mo_bool HasLineTerminator() const { return mHasLineTerminator; }
			const mo_bool HasError() const { return mHasError; }

			TokenType NextToken(Token* token, mo_bool strictMode);
			TokenType NextIdentifier(Token* token, mo_bool strictMode);

			mo_bool ScanRegEx(const UTF8String*& outPattern, const UTF8String*& outFlags, mo_utf8char patternPrefix = 0);
			mo_bool SkipRegEx();

			void Clear();

		private:
			void Append(const mo_int32 ch);
			void Append(const mo_utf8stringptr ptr, mo_int32 len);
			void Next();
			void NextLine();
			void Seek(mo_int32 offset);
			mo_utf8char Peek(mo_int32 offset);

			const UTF8String* CreateIdentifier(const mo_utf8stringptr ptr, mo_int32 len);

			void SetPositionFromCharacterOffset(const mo_utf8stringptr offset) { SetPosition(offset - mCode.start); }
			void SetCodeStart(const mo_utf8stringptr codeStart) { mCode.start = codeStart; }
			void SetInvalidCharacterError();
			
			mo_bool IsLastTokenCompletionKeyword() const;
			mo_bool IsEnd();

			mo_int32 GetOffset() const { return (mCode.current - mCode.start); }
			const mo_utf8char* GetChar() const;

			TokenType ParseKeyword(TokenData* data);
			TokenType ParseIdentifier(TokenData* data, mo_bool strictMode);
			TokenType ParseIdentifierImpl(TokenData* data, mo_bool strictMode);

			mo_bool ParseString(TokenData* data, mo_bool strictMode);
			mo_bool ParseStringImpl(TokenData* data, mo_bool strictMode);

			mo_int32 ParseUnicodeHex();
			
			mo_bool ParseHex(mo_double& value);
			mo_bool ParseOctal(mo_double& value);
			mo_bool ParseDecimal(mo_double& value);
			mo_bool ParseNumberAfterDecimalPoint();
			mo_bool ParseNumberAfterExponentIndicator();
			mo_bool ParseMultilineComment();

		private:
			const SourceCode* mSource;
			GlobalContext* mGlobalContext;
			CodeInfo mCode;
			TokenType mLastToken;
			mo_int32 mLastLineNumber;
			mo_int32 mLineNumber;
			mo_utf8char mCurrentChar;
			mo_bool mIsReparsing;
			mo_bool mIsLineBegin;
			mo_bool mHasLineTerminator;
			mo_bool mHasError;
			UTF8String mError;
			std::vector<mo_utf8char> mBuffer;
	};

	mo_inline const UTF8String* Lexer::CreateIdentifier(const mo_utf8stringptr ptr, mo_int32 len)
	{
		return new UTF8String(ptr, len);
	}
}

#endif