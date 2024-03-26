#include "scanner.h"

namespace css
{
static inline bool is_digit(u32 c)
{
	return c >= '0' && c <= '9';
}

static inline bool is_hex_digit(u32 c)
{
	return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline bool is_uppercase_letter(u32 c)
{
	return c >= 'A' && c <= 'Z';
}

static inline bool is_lowercase_letter(u32 c)
{
	return c >= 'a' && c <= 'z';
}

static inline bool is_letter(u32 c)
{
	return is_uppercase_letter(c) || is_lowercase_letter(c);
}

static inline bool is_non_ascii(u32 c)
{
	return c >= 80;
}

static inline bool is_ident_start(u32 c)
{
	return is_letter(c) || is_non_ascii(c) || c == '_';
}

static inline bool is_ident(u32 c)
{
	return is_ident_start(c) || is_digit(c) || c == '-';
}

static inline bool is_non_printable(u32 c)
{
	return (c >= 0 && c <= 8) || c == 0xb || (c >= 0xe && c <= 0x1f) || c == 0x7f;
}

static inline bool is_newline(u32 c)
{
	return c == '\n';
}

static inline bool is_whitespace(u32 c)
{
	return c == '\n' || c == '\t' || c == ' ';
}

static inline bool is_valid_escape(u32 first, u32 second)
{
	if (first != '\\')
		return false;

	if (is_newline(second))
		return false;

	return true;
}

static Token make_delim(u32 c)
{
	std::string str;
	str.push_back((char) c);
	return {DELIM, str};
}

Scanner::Scanner(const std::string &input) :
    input(input)
{ }

std::vector<Token> Scanner::tokenize()
{
	std::vector<Token> tokens;
	while (auto token = next())
		tokens.push_back(token);

	return tokens;
}

// 4.3.1. Consume a token
// https://www.w3.org/TR/css-syntax-3/#consume-token
Token Scanner::next()
{
	consume_comments();
	consume_next_code_point();

	if (is_whitespace(current_codepoint))
	{
		consume_whitespace();
		return {WHITESPACE};
	}

	if (current_codepoint == '"')
	{
		return consume_string();
	}

	if (current_codepoint == '#')
	{
		if (is_ident(next_codepoint()) || starts_with_valid_escape())
		{
			auto token = Token(HASH);
			if (would_start_ident_sequence())
			{
				token.m_flag = true;
			}

			token.m_value = consume_ident_sequence();
			return token;
		}

		return make_delim(current_codepoint);
	}

	if (current_codepoint == '\'')
	{
		return consume_string();
	}

	if (current_codepoint == '(')
	{
		return {OPEN_PAREN};
	}

	if (current_codepoint == ')')
	{
		return {CLOSE_PAREN};
	}

	if (current_codepoint == '+')
	{
		if (is_digit(next_codepoint()))
		{
			reconsume_current_code_point();
			return consume_numeric();
		}

		return make_delim(current_codepoint);
	}

	if (current_codepoint == ',')
	{
		return {COMMA};
	}

	if (current_codepoint == '-')
	{
		if (is_digit(next_codepoint()))
		{
			reconsume_current_code_point();
			return consume_numeric();
		}

		if (next_code_points_are({'-', '>'}))
		{
			consume_next_code_point();
			consume_next_code_point();
			return {CDC};
		}

		if (would_start_ident_sequence())
		{
			reconsume_current_code_point();
			return consume_ident_like();
		}

		return make_delim(current_codepoint);
	}

	if (current_codepoint == '.')
	{
		if (is_digit(next_codepoint()))
		{
			reconsume_current_code_point();
			return consume_numeric();
		}

		return make_delim(current_codepoint);
	}

	if (current_codepoint == ':')
	{
		return {COLON};
	}

	if (current_codepoint == ';')
	{
		return {SEMICOLON};
	}

	if (current_codepoint == '<')
	{
		if (next_code_points_are({'!', '-', '-'}))
		{
			consume_next_code_point();
			consume_next_code_point();
			consume_next_code_point();
			return {CDO};
		}

		return make_delim(current_codepoint);
	}

	if (current_codepoint == '@')
	{
		if (would_start_ident_sequence())
		{
			return {AT_KEYWORD, consume_ident_sequence()};
		}

		return make_delim(current_codepoint);
	}

	if (current_codepoint == '[')
	{
		return {OPEN_SQUARE};
	}

	if (current_codepoint == '\\')
	{ }

	if (current_codepoint == ']')
	{
		return {CLOSE_SQUARE};
	}

	if (current_codepoint == '{')
	{
		return {OPEN_CURLY};
	}

	if (current_codepoint == '}')
	{
		return {CLOSE_CURLY};
	}

	if (is_digit(current_codepoint))
	{
		reconsume_current_code_point();
		return consume_numeric();
	}

	if (is_ident_start(current_codepoint))
	{
		reconsume_current_code_point();
		return consume_ident_like();
	}

	if (current_codepoint == '\0')
	{
		return {};
	}

	return make_delim(current_codepoint);
}

void Scanner::consume_comments()
{
	if (next_code_points_are({'/', '*'}))
	{
		consume_next_code_point();
		consume_next_code_point();

		while (1)
		{
			if (current_codepoint == '\0')
				return;

			if (next_code_points_are({'*', '/'}))
			{
				consume_next_code_point();
				consume_next_code_point();
				return;
			}

			consume_next_code_point();
		}
	}
}

void Scanner::consume_whitespace()
{
	while (is_whitespace(next_codepoint()))
		consume_next_code_point();
}

bool Scanner::would_start_ident_sequence() const
{
	u32 first, second, third;
	if (pos + 2 >= input.size())
		return false;

	first = next_codepoint();
	second = input[pos + 1];
	third = input[pos + 2];

	if (first == '-')
	{
		if (is_ident_start(second) || second == '-' || is_valid_escape(second, third))
		{
			return true;
		}
		return false;
	}

	if (is_ident_start(first))
		return true;

	if (first == '\\')
	{
		return is_valid_escape(first, second);
	}

	return false;
}

bool Scanner::starts_with_valid_escape() const
{
	return is_valid_escape(current_codepoint, next_codepoint());
}

Token Scanner::consume_string()
{
	return consume_string(current_codepoint);
}

Token Scanner::consume_string(u32 ending_codepoint)
{
	std::string str;
	while (1)
	{
		consume_next_code_point();

		if (current_codepoint == ending_codepoint)
			return {STRING, str};

		if (current_codepoint == '\0')
			return {STRING, str};

		if (is_newline(current_codepoint))
			;

		if (current_codepoint == '\\')
		{
			if (next_codepoint() == '\0')
				continue;

			if (is_newline(next_codepoint()))
				consume_next_code_point();

			else if (starts_with_valid_escape())
			{
				str += consume_escape();
			}

			continue;
		}

		str += current_codepoint;
	}
}

std::string Scanner::consume_ident_sequence()
{
	std::string result;

	while (1)
	{
		consume_next_code_point();

		if (is_ident(current_codepoint))
		{
			result += current_codepoint;
		}

		else if (starts_with_valid_escape())
		{
			result += consume_escape();
		}

		else
		{
			reconsume_current_code_point();
			return result;
		}
	}
}

void Scanner::consume_next_code_point()
{
	current_codepoint = next_codepoint();
	pos++;
}

void Scanner::reconsume_current_code_point()
{
	pos--;
	current_codepoint = input[pos - 1];
}

bool Scanner::is_eof() const
{
	return pos >= input.size();
}

u32 Scanner::next_codepoint() const
{
	if (is_eof())
		return '\0';

	return input[pos];
}

bool Scanner::next_code_points_are(const std::initializer_list<char> &chars) const
{
	if (pos + chars.size() >= input.size())
		return false;

	int i = 0;
	for (char c : chars)
	{
		if (input[pos + i++] != c)
			return false;
	}

	return true;
}

Token Scanner::consume_numeric()
{
	auto number = consume_number();

	if (would_start_ident_sequence())
	{
		return {DIMENSION, number + consume_ident_sequence()};
	}

	if (next_codepoint() == '%')
	{
		consume_next_code_point();
		return {PERCENTAGE, number};
	}

	return {NUMBER, number};
}

std::string Scanner::consume_number()
{
	std::string repr;

	if (next_codepoint() == '+' || next_codepoint() == '-')
	{
		repr += next_codepoint();
		consume_next_code_point();
	}

	while (is_digit(next_codepoint()))
	{
		repr += next_codepoint();
		consume_next_code_point();
	}

	return repr;
}

Token Scanner::consume_ident_like()
{
	auto string = consume_ident_sequence();

	if (string == "url")
	{ }

	if (next_codepoint() == '(')
	{
		consume_next_code_point();
		return {FUNCTION, string};
	}

	return {IDENT, string};
}

u32 Scanner::consume_escape()
{
	consume_next_code_point();

	if (is_hex_digit(current_codepoint))
	{ }

	if (current_codepoint == '\0')
	{
		return U'\ufffd';
	}

	return current_codepoint;
}

}
