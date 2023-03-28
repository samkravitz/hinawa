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
struct ParserBlock
{
	Token associated_token;
	std::vector<Token> value;
};

struct ParserRule
{
	std::vector<Token> prelude;
	ParserBlock block;
};

struct ParserStylesheet
{
	std::vector<ParserRule> rules;
};

struct ParserDeclaration
{
	std::string name;
	std::vector<Token> value;
	bool important{ false };
};

class Parser
{
public:
	Parser() = delete;
	static ParserStylesheet parse_stylesheet(const std::string &input, std::optional<Url> location = {});
	static std::vector<Declaration> parse_inline(std::string);

private:
	Parser(const std::string &);
	Token current_input_token;
	std::vector<Token> tokens;
	std::vector<Token>::iterator pos;    // points to next token

	ComponentValue parse_component_value();
	std::vector<ParserRule> consume_list_of_rules(bool top_level = false);
	std::optional<ParserRule> consume_qualified_rule();
	std::vector<ParserDeclaration> consume_declaration_list();
	std::optional<ParserDeclaration> consume_declaration();
	Token consume_component_value();
	ParserBlock consume_simple_block();

	std::vector<Token> normalize(const std::string &);
	Token next_input_token();
	void consume_next_input_token();
	void reconsume_current_input_token();
};
};
