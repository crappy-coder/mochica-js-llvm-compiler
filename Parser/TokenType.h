#ifndef MOENJIN_PARSER_TOKENTYPE_H
#define MOENJIN_PARSER_TOKENTYPE_H

namespace MoEnjin
{
	enum {
		UnaryOpToken = 64,
		KeywordToken = 128,
		BinaryOpTokenPrecedenceShift = 8,
		BinaryOpTokenAllowsInPrecedenceAdditionalShift = 4,
		BinaryOpTokenPrecedenceMask = 15 << BinaryOpTokenPrecedenceShift
	};

#define BINARY_OP_PRECEDENCE(prec) (((prec) << BinaryOpTokenPrecedenceShift) | ((prec) << (BinaryOpTokenPrecedenceShift + BinaryOpTokenAllowsInPrecedenceAdditionalShift)))
#define IN_OP_PRECEDENCE(prec) ((prec) << (BinaryOpTokenPrecedenceShift + BinaryOpTokenAllowsInPrecedenceAdditionalShift))

	enum TokenType
	{
		TOK_UNKNOWN = -1,
		TOK_NULL = KeywordToken,
		TOK_TRUE,
		TOK_FALSE,
		TOK_BREAK,
		TOK_CASE,
		TOK_DEFAULT,
		TOK_FOR,
		TOK_NEW,
		TOK_VAR,
		TOK_CONST,
		TOK_CONTINUE,
		TOK_FUNCTION,
		TOK_RETURN,
		TOK_IF,
		TOK_THIS,
		TOK_DO,
		TOK_WHILE,
		TOK_SWITCH,
		TOK_WITH,
		TOK_RESERVED,
		TOK_RESERVED_IF_STRICT,
		TOK_THROW,
		TOK_TRY,
		TOK_CATCH,
		TOK_FINALLY,
		TOK_DEBUGGER,
		TOK_ELSE,
		TOK_OPEN_BRACE= 0,
		TOK_CLOSE_BRACE,
		TOK_OPEN_PAREN,
		TOK_CLOSE_PAREN,
		TOK_OPEN_BRACKET,
		TOK_CLOSE_BRACKET,
		TOK_COMMA,
		TOK_QUESTION,
		TOK_NUMBER,
		TOK_IDENTIFIER,
		TOK_STRING,
		TOK_SEMICOLON,
		TOK_COLON,
		TOK_DOT,
		TOK_ERROR,
		TOK_EOF,
		TOK_EQUAL,
		TOK_PLUS_EQUAL,
		TOK_MINUS_EQUAL,
		TOK_MULT_EQUAL,
		TOK_DIV_EQUAL,
		TOK_LSHIFT_EQUAL,
		TOK_RSHIFT_EQUAL,
		TOK_URSHIFT_EQUAL,
		TOK_AND_EQUAL,
		TOK_MOD_EQUAL,
		TOK_XOR_EQUAL,
		TOK_OR_EQUAL,
		TOK_LAST_UNTAGGED,

		// Begin tagged tokens
		TOK_PLUSPLUS =			 0 | UnaryOpToken,
		TOK_MINUSMINUS =		 1 | UnaryOpToken,
		TOK_EXCLAMATION =		 2 | UnaryOpToken,
		TOK_TILDE =				 3 | UnaryOpToken,
		TOK_AUTO_PLUSPLUS =		 4 | UnaryOpToken,
		TOK_AUTO_MINUSMINUS =	 5 | UnaryOpToken,
		TOK_TYPEOF =			 6 | UnaryOpToken | KeywordToken,
		TOK_VOID =				 7 | UnaryOpToken | KeywordToken,
		TOK_DELETE =			 8 | UnaryOpToken | KeywordToken,

		TOK_OR =				 0 | BINARY_OP_PRECEDENCE(1),
		TOK_AND =				 1 | BINARY_OP_PRECEDENCE(2),
		TOK_BIT_OR =			 2 | BINARY_OP_PRECEDENCE(3),
		TOK_BIT_XOR =			 3 | BINARY_OP_PRECEDENCE(4),
		TOK_BIT_AND =			 4 | BINARY_OP_PRECEDENCE(5),
		TOK_EQEQ =				 5 | BINARY_OP_PRECEDENCE(6),
		TOK_NE =				 6 | BINARY_OP_PRECEDENCE(6),
		TOK_STREQ =				 7 | BINARY_OP_PRECEDENCE(6),
		TOK_STRNEQ =			 8 | BINARY_OP_PRECEDENCE(6),
		TOK_LT =				 9 | BINARY_OP_PRECEDENCE(7),
		TOK_GT =				10 | BINARY_OP_PRECEDENCE(7),
		TOK_LE =				11 | BINARY_OP_PRECEDENCE(7),
		TOK_GE =				12 | BINARY_OP_PRECEDENCE(7),
		TOK_INSTANCEOF =		13 | BINARY_OP_PRECEDENCE(7) | KeywordToken,
		TOK_IN =				14 | IN_OP_PRECEDENCE(7)	 | KeywordToken,
		TOK_LSHIFT =			15 | BINARY_OP_PRECEDENCE(8),
		TOK_RSHIFT =			16 | BINARY_OP_PRECEDENCE(8),
		TOK_URSHIFT =			17 | BINARY_OP_PRECEDENCE(8),
		TOK_PLUS =				18 | BINARY_OP_PRECEDENCE(9) | UnaryOpToken,
		TOK_MINUS =				19 | BINARY_OP_PRECEDENCE(9) | UnaryOpToken,
		TOK_TIMES =				20 | BINARY_OP_PRECEDENCE(10),
		TOK_DIVIDE =			21 | BINARY_OP_PRECEDENCE(10),
		TOK_MOD =				22 | BINARY_OP_PRECEDENCE(10)
	};
}

#endif