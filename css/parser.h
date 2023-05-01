#pragma once

#include <optional>
#include <string>

#include "component_value.h"
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
	static Stylesheet parse_stylesheet(const std::string &input, std::optional<Url> location = {});
	static std::vector<Declaration> parse_inline(std::string);

private:
	Parser(const std::string &);
	Token current_input_token;
	std::vector<Token> tokens;
	std::vector<Token>::iterator pos;    // points to next token

	ComponentValue parse_component_value();
	std::vector<Rule> consume_list_of_rules(bool top_level = false);
	std::optional<QualifiedRule> consume_qualified_rule();
	std::vector<Declaration> consume_declaration_list();
	std::optional<Declaration> consume_declaration();
	ComponentValue consume_component_value();
	ParserBlock consume_simple_block();

	std::vector<Token> normalize(const std::string &);
	Token next_input_token();
	void consume_next_input_token();
	void reconsume_current_input_token();
};
};
