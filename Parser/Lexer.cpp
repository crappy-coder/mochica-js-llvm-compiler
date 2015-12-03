#include "Parser/Lexer.h"
#include "Parser/LexerCharacterTypes.h"
#include "Runtime/GlobalContext.h"
#include "Runtime/KnownIdentifiers.h"
#include "Runtime/SourceCode.h"
#include "Text/ASCII.h"
#include "Text/Text.h"
#include "Core/Numbers.h"

namespace MoEnjin
{
	static const mo_int32 LEX_INITIAL_BUFFER_CAPACITY = 32;
	static const mo_int32 LEX_MAX_KEYWORD_LENGTH = 11;

	Lexer::Lexer(GlobalContext* globalContext, const SourceCode& source)
		: mGlobalContext(globalContext)
		, mSource(&source)
		, mIsReparsing(false)
	{
		SetSource(source);
	}

	Lexer::~Lexer() { }

	void Lexer::SetSource(const SourceCode& source)
	{
		if(source.IsNull())
			mCode.start = 0;
		else
			SetCodeStart(source.GetSource().GetChars());

		mSource = &source;
		mCode.current = mCode.start + source.GetBeginOffset();
		mCode.end = mCode.start + source.GetEndOffset();
		mCurrentChar = 0;
		mLineNumber = source.GetStartLine();
		mLastToken = TOK_UNKNOWN;
		mHasError = false;
		mIsLineBegin = true;
		mError.Clear();
		mBuffer.reserve(LEX_INITIAL_BUFFER_CAPACITY);

		if(mCode.current < mCode.end)
			mCurrentChar = *mCode.current;

		MO_ASSERT(GetOffset() == source.GetBeginOffset());
	}

	void Lexer::SetPosition(int position)
	{
		mHasError = mo_false;
		mError.Clear();
		mCode.current = mCode.start + position;
		mCurrentChar = 0;
		mBuffer.resize(0);

		if(mCode.current < mCode.end)
			mCurrentChar = *mCode.current;
	}

	const mo_bool Lexer::IsNextTokenColon() const
	{
		const mo_utf8stringptr code = mCode.current;

		while(code < mCode.end && (IsWhiteSpace(*code) || IsLineTerminator(*code)))
			code++;

		return (code < mCode.end && *code == ':');
	}

	TokenType Lexer::NextToken(Token* token, mo_bool strictMode)
	{
		MO_ASSERT(!mHasError);
		MO_ASSERT(mBuffer.empty());

		mHasLineTerminator = false;

		TokenType tokenType = TOK_ERROR;

	begin:

		while (IsWhiteSpace(mCurrentChar))
			Next();

		if(IsEnd())
			return TOK_EOF;
    
		token->begin = GetOffset();

		// currently we are only supporting latin-1 characters
		//
		// FIXME: support other character sets
		//
		LexCharacterType type = GetLexerCharacter(mCurrentChar);

		switch (type) 
		{
			case LexCharacterGreater:
			{
				Next();

				if (mCurrentChar == '>') 
				{
					Next();

					if (mCurrentChar == '>') 
					{
						Next();
						if (mCurrentChar == '=') 
						{
							Next();
							tokenType = TOK_URSHIFT_EQUAL;
							break;
						}
						tokenType = TOK_URSHIFT;
						break;
					}
					if (mCurrentChar == '=') 
					{
						Next();
						tokenType = TOK_RSHIFT_EQUAL;
						break;
					}
					tokenType = TOK_RSHIFT;
					break;
				}
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_GE;
					break;
				}
				tokenType = TOK_GT;
				break;
			}
			case LexCharacterEqual:
			{
				Next();
				if (mCurrentChar == '=') 
				{
					Next();
					if (mCurrentChar == '=') 
					{
						Next();
						tokenType = TOK_STREQ;
						break;
					}
					tokenType = TOK_EQEQ;
					break;
				}
				tokenType = TOK_EQUAL;
				break;
			}
			case LexCharacterLess:
			{
				Next();
				if (mCurrentChar == '!' && Peek(1) == '-' && Peek(2) == '-') 
				{
					// <!-- marks the beginning of a line comment (for www usage)
					goto inSingleLineComment;
				}
				if (mCurrentChar == '<') 
				{
					Next();
					if (mCurrentChar == '=') 
					{
						Next();
						tokenType = TOK_LSHIFT_EQUAL;
						break;
					}
					tokenType = TOK_LSHIFT;
					break;
				}
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_LE;
					break;
				}
				tokenType = TOK_LT;
				break;
			}
			case LexCharacterExclamationMark:
			{
				Next();
				if (mCurrentChar == '=') 
				{
					Next();
					if (mCurrentChar == '=') 
					{
						Next();
						tokenType = TOK_STRNEQ;
						break;
					}
					tokenType = TOK_NE;
					break;
				}
				tokenType = TOK_EXCLAMATION;
				break;
			}
			case LexCharacterAdd:
			{
				Next();
				if (mCurrentChar == '+') 
				{
					Next();
					tokenType = (!mHasLineTerminator) ? TOK_PLUSPLUS : TOK_AUTO_PLUSPLUS;
					break;
				}
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_PLUS_EQUAL;
					break;
				}
				tokenType = TOK_PLUS;
				break;
			}
			case LexCharacterSub:
			{
				Next();
				if (mCurrentChar == '-') 
				{
					Next();
					if (mIsLineBegin && mCurrentChar == '>') 
					{
						Next();
						goto inSingleLineComment;
					}
					tokenType = (!mHasLineTerminator) ? TOK_MINUSMINUS : TOK_AUTO_MINUSMINUS;
					break;
				}
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_MINUS_EQUAL;
					break;
				}
				tokenType = TOK_MINUS;
				break;
			}
			case LexCharacterMultiply:
			{
				Next();
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_MULT_EQUAL;
					break;
				}
				tokenType = TOK_TIMES;
				break;
			}
			case LexCharacterSlash:
			{
				Next();
				if (mCurrentChar == '/') 
				{
					Next();
					goto inSingleLineComment;
				}
				if (mCurrentChar == '*') 
				{
					Next();
					if (ParseMultilineComment())
						goto begin;

					mError = "Multiline comment was not closed properly";
					goto returnError;
				}
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_DIV_EQUAL;
					break;
				}
				tokenType = TOK_DIVIDE;
				break;
			}
			case LexCharacterAnd:
			{
				Next();
				if (mCurrentChar == '&') 
				{
					Next();
					tokenType = TOK_AND;
					break;
				}
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_AND_EQUAL;
					break;
				}
				tokenType = TOK_BIT_AND;
				break;
			}
			case LexCharacterXor:
			{
				Next();
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_XOR_EQUAL;
					break;
				}
				tokenType = TOK_BIT_XOR;
				break;
			}
			case LexCharacterModulo:
			{
				Next();
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_MOD_EQUAL;
					break;
				}
				tokenType = TOK_MOD;
				break;
			}
			case LexCharacterOr:
			{
				Next();
				if (mCurrentChar == '=') 
				{
					Next();
					tokenType = TOK_OR_EQUAL;
					break;
				}
				if (mCurrentChar == '|') 
				{
					Next();
					tokenType = TOK_OR;
					break;
				}
				tokenType = TOK_BIT_OR;
				break;
			}
			case LexCharacterOpenParen:
			{
				tokenType = TOK_OPEN_PAREN;
				Next();
				break;
			}
			case LexCharacterCloseParen:
			{
				tokenType = TOK_CLOSE_PAREN;
				Next();
				break;
			}
			case LexCharacterOpenBracket:
			{
				tokenType = TOK_OPEN_BRACKET;
				Next();
				break;
			}
			case LexCharacterCloseBracket:
			{
				tokenType = TOK_CLOSE_BRACKET;
				Next();
				break;
			}
			case LexCharacterComma:
			{
				tokenType = TOK_COMMA;
				Next();
				break;
			}
			case LexCharacterColon:
			{
				tokenType = TOK_COLON;
				Next();
				break;
			}
			case LexCharacterQuestion:
			{
				tokenType = TOK_QUESTION;
				Next();
				break;
			}
			case LexCharacterTilde:
			{
				tokenType = TOK_TILDE;
				Next();
				break;
			}
			case LexCharacterSemicolon:
			{
				Next();
				tokenType = TOK_SEMICOLON;
				break;
			}
			case LexCharacterOpenBrace:
			{
				token->data.intValue = GetOffset();
				Next();
				tokenType = TOK_OPEN_BRACE;
				break;
			}
			case LexCharacterCloseBrace:
			{
				token->data.intValue = GetOffset();
				Next();
				tokenType = TOK_CLOSE_BRACE;
				break;
			}
			case LexCharacterDot:
			{
				Next();
				if (!IsASCIIDigit(mCurrentChar)) 
				{
					tokenType = TOK_DOT;
					break;
				}
				goto inNumberAfterDecimalPoint;
			}
			case LexCharacterZero:
			{
				Next();

				if ((mCurrentChar | 0x20) == 'x' && IsASCIIHexDigit(Peek(1))) 
				{
					ParseHex(token->data.doubleValue);
					tokenType = TOK_NUMBER;
				} 
				else 
				{
					Append('0');

					if (IsASCIIOctalDigit(mCurrentChar)) 
					{
						if (ParseOctal(token->data.doubleValue)) 
						{
							if (strictMode) 
							{
								mError = "Octal escapes are forbidden in strict mode";
								goto returnError;
							}

							tokenType = TOK_NUMBER;
						}
					}
				}
				// Fall through into CharacterNumber
			}
			case LexCharacterNumber:
			{
				if (tokenType != TOK_NUMBER) 
				{
					if (!ParseDecimal(token->data.doubleValue)) 
					{
						if (mCurrentChar == '.') 
						{
							Next();
		inNumberAfterDecimalPoint:
							ParseNumberAfterDecimalPoint();
						}
						if ((mCurrentChar | 0x20) == 'e') 
						{
							if (!ParseNumberAfterExponentIndicator()) 
							{
								mError = "Non-number found after exponent indicator";
								goto returnError;
							}
						}

						mo_uint32 sz = 0;
						token->data.doubleValue = Double::Parse(mBuffer.data(), mBuffer.size(), sz);
					}
					tokenType = TOK_NUMBER;
				}

				// No identifiers allowed directly after numeric literal, e.g. "3in" is bad.
				if (IsIdentifierStart(mCurrentChar)) 
				{
					mError = "At least one digit must occur after a decimal point";
					goto returnError;
				}
				mBuffer.resize(0);
				break;
			}
			case LexCharacterQuote:
			{
				if (!ParseString(&token->data, strictMode))
					goto returnError;

				Next();
				tokenType = TOK_STRING;
				break;
			}
			case LexCharacterIdentifierStart:
				MO_ASSERT(IsIdentifierStart(mCurrentChar));
				// Fall through into CharacterBackSlash.
			case LexCharacterBackSlash:
				tokenType = ParseIdentifier(&token->data, strictMode);
				break;
			case LexCharacterLineTerminator:
			{
				MO_ASSERT(IsLineTerminator(mCurrentChar));
				NextLine();

				mIsLineBegin = true;
				mHasLineTerminator = true;
				goto begin;
			}
			case LexCharacterInvalid:
				SetInvalidCharacterError();
				goto returnError;
			default:
			{
				mError = "Internal Error";
				goto returnError;
			}
		}

		mIsLineBegin = false;
		goto returnToken;

	inSingleLineComment:

		while (!IsLineTerminator(mCurrentChar)) 
		{
			if (IsEnd())
				return TOK_EOF;

			Next();
		}

		NextLine();
		mIsLineBegin = true;
		mHasLineTerminator = true;

		if(!IsLastTokenCompletionKeyword())
			goto begin;

		tokenType = TOK_SEMICOLON;
		// Fall through into returnToken.


	returnToken:
		token->line = mLineNumber;
		token->end = GetOffset();

		mLastToken = tokenType;
		return tokenType;


	returnError:
		mHasError = true;
		token->line = mLineNumber;
		token->end = GetOffset();

		return TOK_ERROR;
	}

	TokenType Lexer::NextIdentifier(Token* token, mo_bool strictMode)
	{
		const mo_utf8stringptr start = mCode.current;
		const mo_utf8stringptr end = mCode.end;
		const mo_utf8stringptr current = start;

		if(current >= end)
		{
			MO_ASSERT(current == end);
			return NextToken(token, strictMode);
		}

		if(!IsASCIIAlpha(*current))
			return NextToken(token, strictMode);

		++current;

		while(current < end)
		{
			if(!IsASCIIAlphaNumeric(*current))
				break;

			++current;
		}

		if(current >= end)
			mCurrentChar = 0;
		else
		{
			if(!IsASCII(*current) || (*current == '\\') || (*current == '_') || (*current == '$'))
				return NextToken(token, strictMode);

			mCurrentChar = *current;
		}

		mCode.current = current;

		token->data.id = CreateIdentifier(start, current - start);
		token->line = mLineNumber;
		token->begin = start - mCode.start;
		token->end = GetOffset();

		mLastToken = TOK_IDENTIFIER;

		return mLastToken;
	}

	mo_bool Lexer::ScanRegEx(const UTF8String*& outPattern, const UTF8String*& outFlags, mo_utf8char patternPrefix)
	{
		MO_ASSERT(mBuffer.empty());

		bool lastCharWasEscape = false;
		bool inBrackets = false;

		if (patternPrefix != 0) 
		{
			MO_ASSERT(!IsLineTerminator(patternPrefix));
			MO_ASSERT(patternPrefix != '/');
			MO_ASSERT(patternPrefix != '[');

			Append(patternPrefix);
		}

		while (true) 
		{
			if (IsLineTerminator(mCurrentChar) || IsEnd()) 
			{
				mBuffer.resize(0);
				return false;
			}

			mo_utf8char prevChar = mCurrentChar;
        
			Next();

			if (prevChar == '/' && !lastCharWasEscape && !inBrackets)
				break;

			Append(prevChar);

			if (lastCharWasEscape) 
			{
				lastCharWasEscape = false;
				continue;
			}

			switch (prevChar) 
			{
				case '[':
					inBrackets = true;
					break;
				case ']':
					inBrackets = false;
					break;
				case '\\':
					lastCharWasEscape = true;
					break;
			}
		}

		outPattern = CreateIdentifier(mBuffer.data(), mBuffer.size());
		mBuffer.resize(0);

		while (IsIdentifierPart(mCurrentChar)) 
		{
			Append(mCurrentChar);
			Next();
		}

		outFlags = CreateIdentifier(mBuffer.data(), mBuffer.size());
		mBuffer.resize(0);

		return true;
	}

	mo_bool Lexer::SkipRegEx()
	{
		bool lastCharWasEscape = false;
		bool inBrackets = false;

		while (true) 
		{
			if (IsLineTerminator(mCurrentChar) || IsEnd())
				return false;

			mo_utf8char prevChar = mCurrentChar;
        
			Next();

			if (prevChar == '/' && !lastCharWasEscape && !inBrackets)
				break;

			if (lastCharWasEscape) 
			{
				lastCharWasEscape = false;
				continue;
			}

			switch (prevChar) 
			{
				case '[':
					inBrackets = true;
					break;
				case ']':
					inBrackets = false;
					break;
				case '\\':
					lastCharWasEscape = true;
					break;
			}
		}

		while (IsIdentifierPart(mCurrentChar))
			Next();

		return true;
	}

	void Lexer::Clear()
	{
		mBuffer.clear();
		mIsReparsing = false;
	}

	void Lexer::Append(const mo_int32 ch)
	{
		MO_ASSERT(ch >= 0x00);
		MO_ASSERT(ch <= 0xFF);

		mBuffer.push_back(ch);
	}

	void Lexer::Append(const mo_utf8stringptr ptr, mo_int32 len)
	{
		mo_int32 bufferLen = mBuffer.size();
		mo_utf8stringptr buffer = mo_null;

		mBuffer.resize(bufferLen + len);
		buffer = mBuffer.data() + bufferLen;

		for(mo_int32 i = 0; i < len; ++i)
			buffer[i] = ptr[i];
	}

	void Lexer::Next()
	{
		mCurrentChar = 0;
		++mCode.current;

		if(mCode.current < mCode.end)
			mCurrentChar = *mCode.current;
	}

	void Lexer::NextLine()
	{
		MO_ASSERT(IsLineTerminator(mCurrentChar));

		mo_utf8char prevChar = mCurrentChar;

		Next();

		if(prevChar + mCurrentChar == '\n' + '\r')
			Next();

		++mLineNumber;
	}

	void Lexer::Seek(mo_int32 offset)
	{
		mCode.current += offset;
		mCurrentChar = *mCode.current;
	}

	mo_utf8char Lexer::Peek(mo_int32 offset)
	{
		MO_ASSERT(offset > 0 && offset < 4);

		const mo_utf8stringptr code = mCode.current + offset;

		return (code < mCode.end ? *code : 0);
	}

	void Lexer::SetInvalidCharacterError()
	{
		switch(mCurrentChar)
		{
			case 0:
				mError = "Invalid Character: '\\0'";
				break;
			case 10:
				mError = "Invalid Character: '\\n'";
				break;
			case 11:
				mError = "Invalid Character: '\\v'";
				break;
			case 13:
				mError = "Invalid Character: '\\r'";
				break;
			case 35:
				mError = "Invalid Character: '#'";
				break;
			case 64:
				mError = "Invalid Character: '@'";
				break;
			case 96:
				mError = "Invalid Character: '`'";
				break;
			default:
				mo_utf8char buf[16];
				snprintf(buf, sizeof(buf), "\\u%04u", static_cast<unsigned>(mCurrentChar));
				mError = "Invalid Character: '";
				mError += buf;
				mError += "'";
				break;
		}
	}

	mo_bool Lexer::IsLastTokenCompletionKeyword() const
	{
		return (mLastToken == TOK_CONTINUE || mLastToken == TOK_BREAK || mLastToken == TOK_RETURN || mLastToken == TOK_THROW);
	}

	mo_bool Lexer::IsEnd()
	{
		MO_ASSERT(!mCurrentChar || mCode.current < mCode.end);

		return (!mCurrentChar && mCode.current == mCode.end);
	}

	const mo_utf8char* Lexer::GetChar() const
	{
		MO_ASSERT(mCode.current <= mCode.end);

		return mCode.current;
	}

	TokenType Lexer::ParseIdentifier(TokenData* data, mo_bool strictMode)
	{
		const ptrdiff_t remainingChars = mCode.end - mCode.current;

		if(remainingChars >= LEX_MAX_KEYWORD_LENGTH)
		{
			TokenType keyword = ParseKeyword(data);

			if(keyword != TOK_IDENTIFIER) 
			{
				MO_ASSERT(data->id);

				return (keyword == TOK_RESERVED_IF_STRICT && !strictMode ? TOK_IDENTIFIER : keyword);
			}
		}

		const mo_utf8stringptr identifierStart = GetChar();
    
		while(IsIdentifierPart(mCurrentChar))
			Next();
    
		if(mCurrentChar == '\\') 
		{
			SetPositionFromCharacterOffset(identifierStart);

			return ParseIdentifierImpl(data, strictMode);
		}

		data->id = CreateIdentifier(identifierStart, (GetChar() - identifierStart));

		if(remainingChars < LEX_MAX_KEYWORD_LENGTH) 
		{
			TokenType identifierToken = GetTokenTypeFromIdentifer(data->id);

			//MO_ASSERT((remainingChars < LEX_MAX_KEYWORD_LENGTH) || (identifierToken == TOK_UNKNOWN));

			if (identifierToken == TOK_UNKNOWN)
				return TOK_IDENTIFIER;

			return (identifierToken != TOK_RESERVED_IF_STRICT) || strictMode ? identifierToken : TOK_IDENTIFIER;
		}

		return TOK_IDENTIFIER;
	}

	TokenType Lexer::ParseIdentifierImpl(TokenData* data, mo_bool strictMode)
	{
		const ptrdiff_t remainingChars = mCode.end - mCode.current;
		const mo_utf8stringptr identifierStart = GetChar();
		bool useBuffer = false;

		while (true) 
		{
			if (IsIdentifierPart(mCurrentChar)) 
			{
				Next();
				continue;
			}

			if (mCurrentChar != '\\')
				break;

			// \uXXXX unicode characters.
			useBuffer = true;

			if (identifierStart != GetChar())
				mBuffer.insert(mBuffer.end(), GetChar() - identifierStart, *identifierStart);

			Next();

			if (mCurrentChar != 'u')
				return TOK_ERROR;

			Next();

			int character = ParseUnicodeHex();

			if (character == -1)
				return TOK_ERROR;

			mo_utf8char ucharacter = static_cast<mo_utf8char>(character);

			if (mBuffer.size() ? !IsIdentifierPart(ucharacter) : !IsIdentifierStart(ucharacter))
				return TOK_ERROR;

			Append(ucharacter);

			identifierStart = GetChar();
		}

		int identifierLength;
		const UTF8String* ident = mo_null;

		if (!useBuffer) 
		{
			identifierLength = GetChar() - identifierStart;
			ident = CreateIdentifier(identifierStart, identifierLength);
		} 
		else 
		{
			if (identifierStart != GetChar())
				mBuffer.insert(mBuffer.end(), GetChar() - identifierStart, *identifierStart);

			ident = CreateIdentifier(mBuffer.data(), mBuffer.size());
		}

		data->id = ident;

		if (!useBuffer) 
		{
			// Keywords must not be recognized if there was an \uXXXX in the identifier.
			if (remainingChars < LEX_MAX_KEYWORD_LENGTH) 
			{
				TokenType identifierToken = GetTokenTypeFromIdentifer(data->id);

				//MO_ASSERT((remainingChars < LEX_MAX_KEYWORD_LENGTH) || (identifierToken == TOK_UNKNOWN));

				if (identifierToken == TOK_UNKNOWN)
					return TOK_IDENTIFIER;

				return (identifierToken != TOK_RESERVED_IF_STRICT) || strictMode ? identifierToken : TOK_IDENTIFIER;
			}

			return TOK_IDENTIFIER;
		}

		mBuffer.resize(0);

		return TOK_IDENTIFIER;
	}

	mo_bool Lexer::ParseString(TokenData* data, mo_bool strictMode)
	{
		int startingOffset = GetOffset();
		int startingLineNumber = GetLineNumber();
		mo_utf8char stringQuoteCharacter = mCurrentChar;

		Next();

		const mo_utf8stringptr stringStart = GetChar();

		while (mCurrentChar != stringQuoteCharacter) 
		{
			if (mCurrentChar == '\\') 
			{
				if (stringStart != GetChar())
					Append(stringStart, GetChar() - stringStart);

				Next();

				int escape = SingleCharacterEscape(mCurrentChar);

				// Most common escape sequences first
				if (escape) 
				{
					Append(escape);
					Next();
				} 
				else if (IsLineTerminator(mCurrentChar))
					NextLine();
				else if (mCurrentChar == 'x') 
				{
					Next();

					if (!IsASCIIHexDigit(mCurrentChar) || !IsASCIIHexDigit(Peek(1))) 
					{
						mError = "\\x can only be followed by a hex character sequence";
						return false;
					}

					mo_utf8char prevChar = mCurrentChar;

					Next();
					Append(ToHex(prevChar, mCurrentChar));
					Next();
				}
				else 
				{
					SetPosition(startingOffset);
					SetLineNumber(startingLineNumber);

					mBuffer.resize(0);

					return ParseStringImpl(data, strictMode);
				}

				stringStart = GetChar();
				continue;
			}

			if(mCurrentChar < 0xE)
			{
				SetPosition(startingOffset);
				SetLineNumber(startingLineNumber);

				mBuffer.resize(0);

				return ParseStringImpl(data, strictMode);
			}

			Next();
		}

		if (GetChar() != stringStart)
			Append(stringStart, GetChar() - stringStart);

		data->id = CreateIdentifier(mBuffer.data(), mBuffer.size());
		mBuffer.resize(0);

		return true;
	}

	mo_bool Lexer::ParseStringImpl(TokenData* data, mo_bool strictMode)
	{
		mo_utf8char stringQuoteCharacter = mCurrentChar;
		Next();

		const mo_utf8stringptr stringStart = GetChar();

		while (mCurrentChar != stringQuoteCharacter) 
		{
			if (mCurrentChar == '\\') 
			{
				if (stringStart != GetChar())
					Append(stringStart, GetChar() - stringStart);

				Next();

				int escape = SingleCharacterEscape(mCurrentChar);

				// Most common escape sequences first
				if (escape) 
				{
					Append(escape);
					Next();
				} 
				else if (IsLineTerminator(mCurrentChar))
					NextLine();
				else if (mCurrentChar == 'x') 
				{
					Next();

					if (!IsASCIIHexDigit(mCurrentChar) || !IsASCIIHexDigit(Peek(1))) 
					{
						mError = "\\x can only be followed by a hex character sequence";
						return false;
					}

					mo_utf8char prevChar = mCurrentChar;

					Next();
					Append(ToHex(prevChar, mCurrentChar));
					Next();

				} 
				else if (mCurrentChar == 'u') 
				{
					Next();

					int character = ParseUnicodeHex();

					if (character != -1) 
					{
						// TODO : need to support unicode character sequences
						//
						//Append(character);
					}
					else if (mCurrentChar == stringQuoteCharacter) 
						Append('u');
					else 
					{
						mError = "\\u can only be followed by a Unicode character sequence";
						return false;
					}
				}
				else if (strictMode && IsASCIIDigit(mCurrentChar)) 
				{
					// The only valid numeric escape in strict mode is '\0', and this must not be followed by a decimal digit.
					int character1 = mCurrentChar;
					Next();

					if (character1 != '0' || IsASCIIDigit(mCurrentChar)) 
					{
						mError = "The only valid numeric escape in strict mode is '\\0'";
						return false;
					}

					Append(0);
				} 
				else if (!strictMode && IsASCIIOctalDigit(mCurrentChar)) 
				{
					// Octal character sequences
					mo_utf8char character1 = mCurrentChar;
					Next();

					if (IsASCIIOctalDigit(mCurrentChar)) 
					{
						// Two octal characters
						mo_utf8char character2 = mCurrentChar;
						Next();

						if (character1 >= '0' && character1 <= '3' && IsASCIIOctalDigit(mCurrentChar)) 
						{
							Append((character1 - '0') * 64 + (character2 - '0') * 8 + mCurrentChar - '0');
							Next();
						}
						else 
						{
							Append((character1 - '0') * 8 + character2 - '0');
						}
					} 
					else 
					{
						Append(character1 - '0');
					}
				} 
				else if (!IsEnd()) 
				{
					Append(mCurrentChar);
					Next();
				} 
				else 
				{
					mError = "Unterminated string constant";
					return false;
				}

				stringStart = GetChar();
				continue;
			}

			// Fast check for characters that require special handling.
			// Catches 0, \n, \r, 0x2028, and 0x2029 as efficiently
			// as possible, and lets through all common ASCII characters.
			if (((static_cast<unsigned>(mCurrentChar) - 0xE) & 0x2000)) 
			{
				// New-line or end of input is not allowed
				if (IsEnd() || IsLineTerminator(mCurrentChar)) 
				{
					mError = "Unexpected EOF";
					return false;
				}
				// Anything else is just a normal character
			}

			Next();
		}

		if (GetChar() != stringStart)
			Append(stringStart, GetChar() - stringStart);

		data->id = CreateIdentifier(mBuffer.data(), mBuffer.size());

		mBuffer.resize(0);
		return true;
	}

	mo_int32 Lexer::ParseUnicodeHex()
	{
		mo_utf8char a = Peek(1);
		mo_utf8char b = Peek(2);
		mo_utf8char c = Peek(3);

		if(!IsASCIIHexDigit(mCurrentChar) || !IsASCIIHexDigit(a) || !IsASCIIHexDigit(b) || !IsASCIIHexDigit(c))
			return -1;

		int result = ToUnicode(mCurrentChar, a, b, c);

		Next();
		Next();
		Next();
		Next();

		return result;
	}

	mo_bool Lexer::ParseHex(mo_double& value)
	{
		// Optimization: most hexadecimal values fit into 4 bytes.
		mo_uint32 hexValue = 0;
		mo_int32 maximumDigits = 7;

		// Shift out the 'x' prefix.
		Next();
		
		do 
		{
			hexValue = (hexValue << 4) + ToASCIIHexValue(mCurrentChar);
			Next();

			--maximumDigits;
		} 
		while (IsASCIIHexDigit(mCurrentChar) && maximumDigits >= 0);

		if (maximumDigits >= 0) 
		{
			value = hexValue;
			return true;
		}

		// No more place in the hexValue buffer.
		// The values are shifted out and placed into the m_buffer8 vector.
		for (mo_int32 i = 0; i < 8; ++i) 
		{
			 mo_int32 digit = hexValue >> 28;

			 if (digit < 10)
				 Append(digit + '0');
			 else
				 Append(digit - 10 + 'a');

			 hexValue <<= 4;
		}

		while (IsASCIIHexDigit(mCurrentChar)) 
		{
			Append(mCurrentChar);
			Next();
		}

		value = ParseIntOverflow(mBuffer.data(), mBuffer.size(), 16);
		return true;
	}

	mo_bool Lexer::ParseOctal(mo_double& value)
	{
		// Optimization: most octal values fit into 4 bytes.
		mo_uint32 octalValue = 0;
		mo_int32 maximumDigits = 9;

		// Temporary buffer for the digits. Makes easier
		// to reconstruct the input characters when needed.
		mo_utf8char digits[10];

		do 
		{
			octalValue = octalValue * 8 + (mCurrentChar - '0');
			digits[maximumDigits] = mCurrentChar;

			Next();

			--maximumDigits;
		} 
		while (IsASCIIOctalDigit(mCurrentChar) && maximumDigits >= 0);

		if (!IsASCIIDigit(mCurrentChar) && maximumDigits >= 0) 
		{
			value = octalValue;
			return true;
		}

		for (mo_int32 i = 9; i > maximumDigits; --i)
			 Append(digits[i]);

		while (IsASCIIOctalDigit(mCurrentChar)) 
		{
			Append(mCurrentChar);
			Next();
		}

		if (IsASCIIDigit(mCurrentChar))
			return false;

		value = ParseIntOverflow(mBuffer.data(), mBuffer.size(), 8);

		return true;
	}

	mo_bool Lexer::ParseDecimal(mo_double& value)
	{
		// Optimization: most decimal values fit into 4 bytes.
		mo_uint32 decimalValue = 0;

		// Since parseOctal may be executed before parseDecimal,
		// the m_buffer8 may hold ascii digits.
		if (!mBuffer.size()) 
		{
			mo_int32 maximumDigits = 9;
			// Temporary buffer for the digits. Makes easier
			// to reconstruct the input characters when needed.
			mo_utf8char digits[10];

			do 
			{
				decimalValue = decimalValue * 10 + (mCurrentChar - '0');
				digits[maximumDigits] = mCurrentChar;
				
				Next();

				--maximumDigits;
			} 
			while (IsASCIIDigit(mCurrentChar) && maximumDigits >= 0);

			if (maximumDigits >= 0 && mCurrentChar != '.' && (mCurrentChar | 0x20) != 'e') 
			{
				value = decimalValue;
				return true;
			}

			for (mo_int32 i = 9; i > maximumDigits; --i)
				Append(digits[i]);
		}

		while (IsASCIIDigit(mCurrentChar)) 
		{
			Append(mCurrentChar);
			Next();
		}

		return false;
	}

	mo_bool Lexer::ParseNumberAfterDecimalPoint()
	{
		Append('.');

		while (IsASCIIDigit(mCurrentChar)) 
		{
			Append(mCurrentChar);
			Next();
		}

		return true;
	}

	mo_bool Lexer::ParseNumberAfterExponentIndicator()
	{
		Append('e');
		Next();

		if (mCurrentChar == '+' || mCurrentChar == '-') 
		{
			Append(mCurrentChar);
			Next();
		}

		if (!IsASCIIDigit(mCurrentChar))
			return false;

		do 
		{
			Append(mCurrentChar);
			Next();
		} 
		while (IsASCIIDigit(mCurrentChar));

		return true;
	}

	mo_bool Lexer::ParseMultilineComment()
	{
		while (true) 
		{
			while (mCurrentChar == '*') 
			{
				Next();

				if (mCurrentChar == '/') 
				{
					Next();
					return true;
				}
			}

			if (IsEnd())
				return false;

			if (IsLineTerminator(mCurrentChar)) 
			{
				NextLine();
				mHasLineTerminator = true;
			} 
			else
				Next();
		}
	}

#define CHARPAIR_TOUINT16(a, b) \
	((((mo_uint16)(b)) << 8) + (mo_uint16)(a))

#define CHARQUAD_TOUINT32(a, b, c, d) \
	((((mo_uint32)(CHARPAIR_TOUINT16(c, d))) << 16) + CHARPAIR_TOUINT16(a, b))

#define COMPARE_2CHARS(address, char1, char2) \
    (((mo_uint16*)(address))[0] == CHARPAIR_TOUINT16(char1, char2))

#define COMPARE_4CHARS(address, char1, char2, char3, char4) \
    (((mo_uint32*)(address))[0] == CHARQUAD_TOUINT32(char1, char2, char3, char4))

#define COMPARE_3CHARS(address, char1, char2, char3) \
    (COMPARE_2CHARS(address, char1, char2) && ((address)[2] == (char3)))

#define COMPARE_5CHARS(address, char1, char2, char3, char4, char5) \
    (COMPARE_4CHARS(address, char1, char2, char3, char4) && ((address)[4] == (char5)))

#define COMPARE_6CHARS(address, char1, char2, char3, char4, char5, char6) \
    (COMPARE_4CHARS(address, char1, char2, char3, char4) && COMPARE_2CHARS(address + 4, char5, char6))

#define COMPARE_7CHARS(address, char1, char2, char3, char4, char5, char6, char7) \
    (COMPARE_4CHARS(address, char1, char2, char3, char4) && COMPARE_4CHARS(address + 3, char4, char5, char6, char7))

#define COMPARE_8CHARS(address, char1, char2, char3, char4, char5, char6, char7, char8) \
    (COMPARE_4CHARS(address, char1, char2, char3, char4) && COMPARE_4CHARS(address + 4, char5, char6, char7, char8))

	TokenType Lexer::ParseKeyword(TokenData* data)
	{
		MO_ASSERT((mCode.end - mCode.current) >= LEX_MAX_KEYWORD_LENGTH);

		const mo_utf8stringptr code = mCode.current;

		if (code[0] == 'f') {
			if (COMPARE_7CHARS(code + 1, 'u', 'n', 'c', 't', 'i', 'o', 'n')) {
				if (!IsIdentifierPart(code[8])) {
					Seek(8);
					data->id = &KnownIdentifiers::functionKeyword;
					return TOK_FUNCTION;
				}
			} else if (COMPARE_2CHARS(code + 1, 'o', 'r')) {
				if (!IsIdentifierPart(code[3])) {
					Seek(3);
					data->id = &KnownIdentifiers::forKeyword;
					return TOK_FOR;
				}
			} else if (COMPARE_4CHARS(code + 1, 'a', 'l', 's', 'e')) {
				if (!IsIdentifierPart(code[5])) {
					Seek(5);
					data->id = &KnownIdentifiers::falseKeyword;
					return TOK_FALSE;
				}
			} else if (COMPARE_6CHARS(code + 1, 'i', 'n', 'a', 'l', 'l', 'y')) {
				if (!IsIdentifierPart(code[7])) {
					Seek(7);
					data->id = &KnownIdentifiers::finallyKeyword;
					return TOK_FINALLY;
				}
			}
		} else if (code[0] == 't') {
			if (code[1] == 'h') {
				if (COMPARE_2CHARS(code + 2, 'i', 's')) {
					if (!IsIdentifierPart(code[4])) {
						Seek(4);
						data->id = &KnownIdentifiers::thisKeyword;
						return TOK_THIS;
					}
				} else if (COMPARE_4CHARS(code + 1, 'h', 'r', 'o', 'w')) {
					if (!IsIdentifierPart(code[5])) {
						Seek(5);
						data->id = &KnownIdentifiers::throwKeyword;
						return TOK_THROW;
					}
				}
			} else if (code[1] == 'r') {
				if (COMPARE_2CHARS(code + 2, 'u', 'e')) {
					if (!IsIdentifierPart(code[4])) {
						Seek(4);
						data->id = &KnownIdentifiers::trueKeyword;
						return TOK_TRUE;
					}
				} else if (code[2] == 'y') {
					if (!IsIdentifierPart(code[3])) {
						Seek(3);
						data->id = &KnownIdentifiers::tryKeyword;
						return TOK_TRY;
					}
				}
			} else if (COMPARE_5CHARS(code + 1, 'y', 'p', 'e', 'o', 'f')) {
				if (!IsIdentifierPart(code[6])) {
					Seek(6);
					data->id = &KnownIdentifiers::typeofKeyword;
					return TOK_TYPEOF;
				}
			}
		} else if (code[0] == 'i') {
			if (code[1] == 'f') {
				if (!IsIdentifierPart(code[2])) {
					Seek(2);
					data->id = &KnownIdentifiers::ifKeyword;
					return TOK_IF;
				}
			} else if (code[1] == 'n') {
				if (!IsIdentifierPart(code[2])) {
					Seek(2);
					data->id = &KnownIdentifiers::inKeyword;
					return TOK_IN;
				}
				if (COMPARE_7CHARS(code + 2, 't', 'e', 'r', 'f', 'a', 'c', 'e')) {
					if (!IsIdentifierPart(code[9])) {
						Seek(9);
						data->id = &KnownIdentifiers::interfaceKeyword;
						return TOK_RESERVED_IF_STRICT;
					}
				} else if (COMPARE_8CHARS(code + 2, 's', 't', 'a', 'n', 'c', 'e', 'o', 'f')) {
					if (!IsIdentifierPart(code[10])) {
						Seek(10);
						data->id = &KnownIdentifiers::instanceofKeyword;
						return TOK_INSTANCEOF;
					}
				}
			} else if (COMPARE_2CHARS(code + 1, 'm', 'p')) {
				if (COMPARE_4CHARS(code + 2, 'p', 'o', 'r', 't')) {
					if (!IsIdentifierPart(code[6])) {
						Seek(6);
						data->id = &KnownIdentifiers::importKeyword;
						return TOK_RESERVED;
					}
				} else if (COMPARE_7CHARS(code + 3, 'l', 'e', 'm', 'e', 'n', 't', 's')) {
					if (!IsIdentifierPart(code[10])) {
						Seek(10);
						data->id = &KnownIdentifiers::implementsKeyword;
						return TOK_RESERVED_IF_STRICT;
					}
				}
			}
		} else if (code[0] == 'v') {
			if (COMPARE_2CHARS(code + 1, 'a', 'r')) {
				if (!IsIdentifierPart(code[3])) {
					Seek(3);
					data->id = &KnownIdentifiers::varKeyword;
					return TOK_VAR;
				}
			} else if (COMPARE_4CHARS(code, 'v', 'o', 'i', 'd')) {
				if (!IsIdentifierPart(code[4])) {
					Seek(4);
					data->id = &KnownIdentifiers::voidKeyword;
					return TOK_VOID;
				}
			}
		} else if (COMPARE_6CHARS(code, 'r', 'e', 't', 'u', 'r', 'n')) {
			if (!IsIdentifierPart(code[6])) {
				Seek(6);
				data->id = &KnownIdentifiers::returnKeyword;
				return TOK_RETURN;
			}
		} else if (code[0] == 'n') {
			if (COMPARE_4CHARS(code, 'n', 'u', 'l', 'l')) {
				if (!IsIdentifierPart(code[4])) {
					Seek(4);
					data->id = &KnownIdentifiers::nullKeyword;
					return TOK_NULL;
				}
			} else if (COMPARE_2CHARS(code + 1, 'e', 'w')) {
				if (!IsIdentifierPart(code[3])) {
					Seek(3);
					data->id = &KnownIdentifiers::newKeyword;
					return TOK_NEW;
				}
			}
		} else if (code[0] == 'e') {
			if (COMPARE_4CHARS(code, 'e', 'l', 's', 'e')) {
				if (!IsIdentifierPart(code[4])) {
					Seek(4);
					data->id = &KnownIdentifiers::elseKeyword;
					return TOK_ELSE;
				}
			} else if (code[1] == 'x') {
				if (COMPARE_4CHARS(code + 2, 'p', 'o', 'r', 't')) {
					if (!IsIdentifierPart(code[6])) {
						Seek(6);
						data->id = &KnownIdentifiers::exportKeyword;
						return TOK_RESERVED;
					}
				} else if (COMPARE_5CHARS(code + 2, 't', 'e', 'n', 'd', 's')) {
					if (!IsIdentifierPart(code[7])) {
						Seek(7);
						data->id = &KnownIdentifiers::extendsKeyword;
						return TOK_RESERVED;
					}
				}
			} else if (COMPARE_4CHARS(code, 'e', 'n', 'u', 'm')) {
				if (!IsIdentifierPart(code[4])) {
					Seek(4);
					data->id = &KnownIdentifiers::enumKeyword;
					return TOK_RESERVED;
				}
			}
		} else if (code[0] == 'c') {
			if (code[1] == 'a') {
				if (COMPARE_4CHARS(code + 1, 'a', 't', 'c', 'h')) {
					if (!IsIdentifierPart(code[5])) {
						Seek(5);
						data->id = &KnownIdentifiers::catchKeyword;
						return TOK_CATCH;
					}
				} else if (COMPARE_2CHARS(code + 2, 's', 'e')) {
					if (!IsIdentifierPart(code[4])) {
						Seek(4);
						data->id = &KnownIdentifiers::caseKeyword;
						return TOK_CASE;
					}
				}
			} else if (COMPARE_2CHARS(code + 1, 'o', 'n')) {
				if (COMPARE_5CHARS(code + 3, 't', 'i', 'n', 'u', 'e')) {
					if (!IsIdentifierPart(code[8])) {
						Seek(8);
						data->id = &KnownIdentifiers::continueKeyword;
						return TOK_CONTINUE;
					}
				} else if (COMPARE_2CHARS(code + 3, 's', 't')) {
					if (!IsIdentifierPart(code[5])) {
						Seek(5);
						data->id = &KnownIdentifiers::constKeyword;
						return TOK_CONST;
					}
				}
			} else if (COMPARE_4CHARS(code + 1, 'l', 'a', 's', 's')) {
				if (!IsIdentifierPart(code[5])) {
					Seek(5);
					data->id = &KnownIdentifiers::classKeyword;
					return TOK_RESERVED;
				}
			}
		} else if (COMPARE_5CHARS(code, 'b', 'r', 'e', 'a', 'k')) {
			if (!IsIdentifierPart(code[5])) {
				Seek(5);
				data->id = &KnownIdentifiers::breakKeyword;
				return TOK_BREAK;
			}
		} else if (code[0] == 'w') {
			if (COMPARE_4CHARS(code + 1, 'h', 'i', 'l', 'e')) {
				if (!IsIdentifierPart(code[5])) {
					Seek(5);
					data->id = &KnownIdentifiers::whileKeyword;
					return TOK_WHILE;
				}
			} else if (COMPARE_4CHARS(code, 'w', 'i', 't', 'h')) {
				if (!IsIdentifierPart(code[4])) {
					Seek(4);
					data->id = &KnownIdentifiers::withKeyword;
					return TOK_WITH;
				}
			}
		} else if (code[0] == 'd') {
			if (code[1] == 'e') {
				if (COMPARE_5CHARS(code + 2, 'f', 'a', 'u', 'l', 't')) {
					if (!IsIdentifierPart(code[7])) {
						Seek(7);
						data->id = &KnownIdentifiers::defaultKeyword;
						return TOK_DEFAULT;
					}
				} else if (COMPARE_6CHARS(code + 2, 'b', 'u', 'g', 'g', 'e', 'r')) {
					if (!IsIdentifierPart(code[8])) {
						Seek(8);
						data->id = &KnownIdentifiers::debuggerKeyword;
						return TOK_DEBUGGER;
					}
				} else if (COMPARE_4CHARS(code + 2, 'l', 'e', 't', 'e')) {
					if (!IsIdentifierPart(code[6])) {
						Seek(6);
						data->id = &KnownIdentifiers::deleteKeyword;
						return TOK_DELETE;
					}
				}
			} else if (code[1] == 'o') {
				if (!IsIdentifierPart(code[2])) {
					Seek(2);
					data->id = &KnownIdentifiers::doKeyword;
					return TOK_DO;
				}
			}
		} else if (COMPARE_5CHARS(code, 'y', 'i', 'e', 'l', 'd')) {
			if (!IsIdentifierPart(code[5])) {
				Seek(5);
				data->id = &KnownIdentifiers::yieldKeyword;
				return TOK_RESERVED_IF_STRICT;
			}
		} else if (code[0] == 'p') {
			if (code[1] == 'r') {
				if (COMPARE_5CHARS(code + 2, 'i', 'v', 'a', 't', 'e')) {
					if (!IsIdentifierPart(code[7])) {
						Seek(7);
						data->id = &KnownIdentifiers::privateKeyword;
						return TOK_RESERVED_IF_STRICT;
					}
				} else if (COMPARE_7CHARS(code + 2, 'o', 't', 'e', 'c', 't', 'e', 'd')) {
					if (!IsIdentifierPart(code[9])) {
						Seek(9);
						data->id = &KnownIdentifiers::protectedKeyword;
						return TOK_RESERVED_IF_STRICT;
					}
				}
			} else if (COMPARE_5CHARS(code + 1, 'u', 'b', 'l', 'i', 'c')) {
				if (!IsIdentifierPart(code[6])) {
					Seek(6);
					data->id = &KnownIdentifiers::publicKeyword;
					return TOK_RESERVED_IF_STRICT;
				}
			} else if (COMPARE_6CHARS(code + 1, 'a', 'c', 'k', 'a', 'g', 'e')) {
				if (!IsIdentifierPart(code[7])) {
					Seek(7);
					data->id = &KnownIdentifiers::packageKeyword;
					return TOK_RESERVED_IF_STRICT;
				}
			}
		} else if (code[0] == 's') {
			if (COMPARE_5CHARS(code + 1, 'w', 'i', 't', 'c', 'h')) {
				if (!IsIdentifierPart(code[6])) {
					Seek(6);
					data->id = &KnownIdentifiers::switchKeyword;
					return TOK_SWITCH;
				}
			} else if (COMPARE_5CHARS(code + 1, 't', 'a', 't', 'i', 'c')) {
				if (!IsIdentifierPart(code[6])) {
					Seek(6);
					data->id = &KnownIdentifiers::staticKeyword;
					return TOK_RESERVED_IF_STRICT;
				}
			} else if (COMPARE_4CHARS(code + 1, 'u', 'p', 'e', 'r')) {
				if (!IsIdentifierPart(code[5])) {
					Seek(5);
					data->id = &KnownIdentifiers::superKeyword;
					return TOK_RESERVED;
				}
			}
		} else if (COMPARE_3CHARS(code, 'l', 'e', 't')) {
			if (!IsIdentifierPart(code[3])) {
				Seek(3);
				data->id = &KnownIdentifiers::letKeyword;
				return TOK_RESERVED_IF_STRICT;
			}
		}

		return TOK_IDENTIFIER;
	}
}