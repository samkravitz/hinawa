#pragma once

#include <string>
#include <vector>

#include "token.h"
#include "util/hinawa.h"

namespace css
{
class Scanner
{
public:
	Scanner(const std::string &);

	Token next();
	std::vector<Token> tokenize();

private:
	u32 current_codepoint;
	u32 next_codepoint() const;

	std::string input;
	std::size_t pos{0};

	void consume_comments();
	void consume_whitespace();
	Token consume_string();
	Token consume_string(u32);
	u32 consume_escape();
	std::string consume_ident_sequence();
	Token consume_ident_like();
	Token consume_numeric();
	std::string consume_number();
	void consume_next_code_point();
	void reconsume_current_code_point();
	bool would_start_ident_sequence() const;
	bool starts_with_valid_escape() const;
	bool next_code_points_are(const std::initializer_list<char> &) const;
	bool starts_with_ident_sequence() const;
	bool is_eof() const;
};
}
