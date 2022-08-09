#pragma once

#include <string>

namespace css
{
enum TokenType
{
	S = 1,
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
	Token(std::string, TokenType, int);
	Token() = default;

	std::string to_string();

	inline TokenType type() { return m_type; }
	inline std::string value() { return m_value; }
	inline int line() { return m_line; }

private:
	std::string m_value;
	TokenType m_type;
	int m_line;
};
}
