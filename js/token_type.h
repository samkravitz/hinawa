#pragma once

namespace js
{
enum TokenType : int
{
	// One character tokens
	LEFT_PAREN = 1,    // (
	RIGHT_PAREN,       // )
	LEFT_BRACE,        // {
	RIGHT_BRACE,       // }
	COMMA,             // ,
	DOT,               // .
	MINUS,             // -
	PLUS,              // +
	SLASH,             // /
	STAR,              // *
	MOD,               // %
	SEMICOLON,         // ;
	NOT,               // !
	EQUAL,             // =
	GREATER,           // >
	LESS,              // <
	AND,               // &
	PIPE,              // |
	NEWLINE,

	// Two+ character tokens
	NOT_EQUAL,         // !=
	EQUAL_EQUAL,       // ==
	EQUAL_EQUAL_EQUAL, // ===
	NOT_EQUAL_EQUAL,   // !==
	GREATER_EQUAL,     // >=
	LESS_EQUAL,        // <=
	LESS_LESS,         // <<
	GREATER_GREATER,   // >>
	AND_AND,           // &&
	PIPE_PIPE,         // ||

	// Literals
	IDENTIFIER,
	STRING,            // e.g. 'Hello World'
	NUMBER,            // e.g. 3.4, -1

	// Keywords
	KEY_AWAIT,
	KEY_BREAK,
	KEY_CASE,
	KEY_CATCH,
	KEY_CLASS,
	KEY_CONST,
	KEY_CONTINUE,
	KEY_DEBUGGER,
	KEY_DEFAULT,
	KEY_DELETE,
	KEY_DO,
	KEY_ELSE,
	KEY_ENUM,
	KEY_EXPORT,
	KEY_EXTENDS,
	KEY_FALSE,
	KEY_FINALLY,
	KEY_FOR,
	KEY_FUNCTION,
	KEY_IF,
	KEY_IMPLEMENTS,
	KEY_IMPORT,
	KEY_IN,
	KEY_INSTANCEOF,
	KEY_INTERFACE,
	KEY_LET,
	KEY_NEW,
	KEY_NULL,
	KEY_PACKAGE,
	KEY_PRIVATE,
	KEY_PROTECTED,
	KEY_PUBLIC,
	KEY_RETURN,
	KEY_SUPER,
	KEY_SWITCH,
	KEY_STATIC,
	KEY_THIS,
	KEY_THROW,
	KEY_TRY,
	KEY_TRUE,
	KEY_TYPEOF,
	KEY_VAR,
	KEY_VOID,
	KEY_WHILE,
	KEY_WITH,
	KEY_YIELD,

	KEY_EOF
};
}
