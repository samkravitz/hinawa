#pragma once

#include <optional>
#include <string>

#include "component_value.h"
#include "selector.h"
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
	static std::optional<Selector> parse_selector_list(const std::vector<ComponentValue> &);
	static std::optional<Selector> parse_selector(const std::string &);
	static std::vector<Declaration> parse_inline(std::string);
	static std::vector<Declaration> parse_declaration_list(const std::vector<ComponentValue> &);
	static Value *parse_style_value(const std::string &, const std::vector<ComponentValue> &);
	static Value *parse_style_value(const std::string &, const std::string &);

private:
	Parser(const std::string &);
	Parser(const std::vector<ComponentValue> &);
	Token current_input_token;
	std::vector<Token> tokens;
	std::vector<Token>::iterator pos;    // points to next token

	ComponentValue parse_component_value();
	std::vector<ComponentValue> parse_list_of_component_values();
	std::vector<Rule> consume_list_of_rules(bool top_level = false);
	std::optional<QualifiedRule> consume_qualified_rule();
	std::vector<Declaration> consume_declaration_list();
	std::optional<Declaration> consume_declaration();
	ComponentValue consume_component_value();
	ParserBlock consume_simple_block();

	std::optional<Selector> parse_complex_selector_list();
	std::optional<Selector::ComplexSelector> parse_complex_selector();
	std::optional<Selector> parse_compound_selector_list();
	std::optional<Selector::CompoundSelector> parse_compound_selector();
	std::optional<Selector::SimpleSelector> parse_simple_selector();
	std::optional<Selector::SimpleSelector::AttributeSelector> parse_attribute_selector();
	std::optional<Selector::Combinator> parse_combinator();

	std::vector<Token> normalize(const std::string &);
	Token next_input_token();
	void consume_next_input_token();
	void reconsume_current_input_token();
	void skip_whitespace();

	std::vector<Token>::iterator save_state() const;
	void restore_state(const std::vector<Token>::iterator &);
};
};
