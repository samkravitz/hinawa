#pragma once

#include <optional>
#include <string>

#include "scanner.h"
#include "stylesheet.h"
#include "token.h"
#include "value.h"
#include "web/url.h"

namespace css
{
class Parser
{
public:
	Parser() = delete;
	static Stylesheet parse(std::string);
	static Stylesheet parse_stylesheet(const std::string &input, std::optional<Url> location = {});
	static std::vector<Declaration> parse_inline(std::string);

private:
	Parser(const std::string &);
	Token current_input_token;
	std::vector<Token> tokens;
	std::vector<Token>::iterator pos; // points to next token

	std::vector<Token> normalize(const std::string &);
	void consume_next_input_token();
	void reconsume_current_input_token();

	std::vector<Rule> consume_list_of_rules(bool top_level = false);
};
};
