#pragma once

#include <string>

namespace css
{
enum TokenType
{
	Eof = 0,
	S,
	CDO,
	CDC,
	INCLUDES,
	DASHMATCH,
	STRING,
	BAD_STRING,
	IDENT,
	HASH,
	IMPORT_SYM,
	PAGE_SYM,
	MEDIA_SYM,
	CHARSET_SYM,
	IMPORTANT_SYM,
	EMS,
	EXS,
	LENGTH,
	ANGLE,
	TIME,
	FREQ,
	DIMENSION,
	PERCENTAGE,
	NUMBER,
	URI,
	BAD_URI,
	FUNCTION,
	COMMA,
	COLON,
	SEMICOLON,
	OPEN_BRACE,
	CLOSE_BRACE,
};

class Token
{
public:
	Token();
	Token(std::string, TokenType, int);

	operator bool() const { return m_type != Eof; }

	bool is_whitespace() const { return false; }
	bool is_eof() const { return m_type == Eof; }

	std::string to_string();

	inline TokenType type() const { return m_type; }
	inline std::string value() const { return m_value; }
	inline int line() const { return m_line; }

private:
	std::string m_value;
	TokenType m_type;
	int m_line;
};
}
