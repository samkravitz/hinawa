#pragma once

#include <string>

namespace css
{
enum TokenType
{
	Eof = 0,
	IDENT,
	FUNCTION,
	AT_KEYWORD,
	HASH,
	STRING,
	BAD_STRING,
	URL,
	BAD_URL,
	DELIM,
	NUMBER,
	PERCENTAGE,
	DIMENSION,
	WHITESPACE,
	CDO,
	CDC,
	COLON,
	SEMICOLON,
	COMMA,
	OPEN_SQUARE,
	CLOSE_SQUARE,
	OPEN_PAREN,
	CLOSE_PAREN,
	OPEN_CURLY,
	CLOSE_CURLY,
};

class Token
{
friend class Scanner;

public:
	Token() :
	    Token(Eof)
	{ }

	Token(TokenType type) :
	    m_type(type)
	{
		switch (type)
		{
			case COMMA: m_value = ","; break;
			case COLON: m_value = ":"; break;
			case SEMICOLON: m_value = ";"; break;
			case OPEN_CURLY: m_value = "{"; break;
			case CLOSE_CURLY: m_value = "}"; break;
			case OPEN_SQUARE: m_value = "["; break;
			case CLOSE_SQUARE: m_value = "]"; break;
			case OPEN_PAREN: m_value = "("; break;
			case CLOSE_PAREN: m_value = ")"; break;
			default: break;
		}
	}

	Token(TokenType type, std::string value) :
	    m_type(type),
	    m_value(value)
	{ }

	operator bool() const { return m_type != Eof; }

	bool is_whitespace() const { return false; }
	bool is_eof() const { return m_type == Eof; }

	std::string to_string();

	inline TokenType type() const { return m_type; }
	inline std::string value() const { return m_value; }

private:
	TokenType m_type;
	std::string m_value;
	bool m_flag{ false };    // true if id, false if unrestricted
	std::string m_unit;
};
}
