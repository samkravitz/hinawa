// https://www.w3.org/TR/css-syntax-3/#parsing
#include "parser.h"
#include "component_value.h"
#include "stylesheet.h"

#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <sstream>

namespace css
{
Parser::Parser(const std::string &input)
{
	Scanner scanner(input.c_str());
	tokens = scanner.tokenize();
	pos = tokens.begin();
}

Parser::Parser(const std::vector<ComponentValue> &input)
{
	std::vector<Token> ts;
	for (const auto &t : input)
		ts.push_back(t.token);

	tokens = ts;
	pos = tokens.begin();
}

/**
* @brief parse a style value from the components of a declaration
* @param name declaration's name
* @param value list of tokens comprising the declaration's value
*
*/
Value *Parser::parse_style_value(const std::string &name, const std::vector<ComponentValue> &value)
{
	std::string value_text{};
	for (const auto &cv : value)
		value_text += cv.token.value();

	if (name == "display")
	{
		return new Keyword(value_text);
	}

	if (name == "color")
	{
		if (auto *color = Color::from_color_string(value_text))
			return color;
	}

	return nullptr;
}

// 5.3.3. Parse a stylesheet
// https://www.w3.org/TR/css-syntax-3/#parse-stylesheet
Stylesheet Parser::parse_stylesheet(const std::string &input, std::optional<Url> location)
{
	// To parse a stylesheet from an input given an optional url location:

	// 1. If input is a byte stream for stylesheet, decode bytes from input, and set input to the result
	// 2. Normalize input, and set input to the result
	Parser parser(input);
	//parser.normalize(input);

	// 3. Create a new stylesheet, with its location set to location (or null, if location was not passed).
	Stylesheet stylesheet = {};

	// 4. Consume a list of rules from input, with the top-level flag set, and set the stylesheet's value to the result
	stylesheet.rules = parser.consume_list_of_rules(true);

	// 5. Return the stylesheet
	return stylesheet;
}

// 5.3.1 Parse something according to a CSS grammar
// https://www.w3.org/TR/css-syntax-3/#css-parse-something-according-to-a-css-grammar
std::optional<Selector> Parser::parse_selector_list(const std::vector<ComponentValue> &component_values)
{
	Parser parser(component_values);

	// 1. Normalize input, and set input to the result.

	// 2. Parse a list of component values from input, and let result be the return value.

	// 3. Attempt to match result against grammar. If this is successful, return the matched result; otherwise, return failure.

	return parser.parse_complex_selector_list();
}

// 5.3.8. Parse a list of declarations
// https://www.w3.org/TR/css-syntax-3/#parse-a-list-of-declarations
std::vector<Declaration> Parser::parse_declaration_list(const std::vector<ComponentValue> &component_values)
{
	// 1. Normalize input, and set input to the result.
	Parser parser(component_values);

	// 2. Consume a list of declarations from input, and return the result.
	return parser.consume_declaration_list();
}

// 5.3.9. Parse a component value
// https://www.w3.org/TR/css-syntax-3/#parse-component-value
ComponentValue Parser::parse_component_value()
{
	// 1. Normalize input, and set input to the result.
	// 2. While the next input token from input is a <whitespace-token>, consume the next input token from input.
	while (next_input_token().is_whitespace())
		consume_next_input_token();

	// 3. If the next input token from input is an <EOF-token>, return a syntax error.
	if (next_input_token().is_eof())
		fmt::print(stderr, "parse_component_value\n");

	// 4. Consume a component value from input and let value be the return value.
	auto value = consume_component_value();

	// 5. While the next input token from input is a <whitespace-token>, consume the next input token.
	while (next_input_token().is_whitespace())
		consume_next_input_token();

	// 6. If the next input token from input is an <EOF-token>, return value. Otherwise, return a syntax error.
	if (next_input_token().is_eof())
		return value;

	fmt::print(stderr, "parse_component_value\n");
	return {};
}

// 5.4.1. Consume a list of rules
// https://www.w3.org/TR/css-syntax-3/#consume-a-list-of-rules
std::vector<Rule> Parser::consume_list_of_rules(bool top_level)
{
	// To consume a list of rules, given a top-level flag:

	// Create an initially empty list of rules
	std::vector<Rule> rules;

	// Repeatedly consume the next input token:
	while (1)
	{
		consume_next_input_token();

		// <whitespace-token>
		if (current_input_token.is_whitespace())
		{
			// Do nothing
			continue;
		}

		// <EOF-token>
		if (current_input_token.is_eof())
		{
			// Return the list of rules
			return rules;
		}

		// <CDO-token>
		// <CDC-token>
		if (current_input_token.type() == CDO || current_input_token.type() == CDC)
		{
			// If the top-level flag is set, do nothing
			if (top_level)
				continue;

			// Otherwise, reconsume the current input token
			reconsume_current_input_token();

			// Consume a qualified rule. If anything is returned, append it to the list of rules
			if (auto qualified_rule = consume_qualified_rule())
			{
				Rule rule = {};
				rule.qualified_rule = *qualified_rule;
				rules.push_back(rule);
			}

			continue;
		}

		// <at-keyword_token>

		// anything else
		// Reconsume the current input token.
		reconsume_current_input_token();

		// Consume a qualified rule. If anything is returned, append it to the list of rules.
		if (auto qualified_rule = consume_qualified_rule())
		{
			Rule rule = {};
			rule.qualified_rule = *qualified_rule;
			rules.push_back(rule);
		}
	}
}

// 5.4.3. Consume a qualified rule
// https://www.w3.org/TR/css-syntax-3/#consume-a-qualified-rule
std::optional<QualifiedRule> Parser::consume_qualified_rule()
{
	// Create a new qualified rule with its prelude initially set to an empty list, and its value initially set to nothing
	auto rule = QualifiedRule{};

	// Repeatedly consume the next input token:
	while (1)
	{
		consume_next_input_token();

		// <EOF-token>
		if (!current_input_token)
		{
			// This is a parse error. Return nothing.
			return {};
		}

		// <{-token>
		if (current_input_token.type() == OPEN_CURLY)
		{
			// Consume a simple block and assign it to the qualified rule's block. Return the qualified rule.
			rule.block = consume_simple_block();
			return rule;
		}

		// anything else
		reconsume_current_input_token();
		rule.prelude.push_back(consume_component_value());
	}
}

// 5.4.5. Consume a list of declarations
// https://www.w3.org/TR/css-syntax-3/#consume-list-of-declarations
std::vector<Declaration> Parser::consume_declaration_list()
{
	// Create an initially empty list of declarations
	std::vector<Declaration> declarations;

	// Repeatedly consume the next input token:
	while (1)
	{
		consume_next_input_token();

		//<whitespace-token>
		// <semicolon-token>
		if (current_input_token.is_whitespace() || current_input_token.type() == SEMICOLON)
		{
			// Do nothing
			continue;
		}

		// <EOF-token>
		if (!current_input_token)
		{
			return declarations;
		}

		// <at-keyword-token>
		if (current_input_token.value() == "@")
		{
			// Reconsume the current input token.
			reconsume_current_input_token();

			// Consume an at-rule. Append the returned rule to the list of declarations.
		}

		// <ident-token>
		if (current_input_token.type() == IDENT)
		{
			// Initialize a temporary list initially filled with the current input token
			std::vector<ComponentValue> temp_list = {ComponentValue(current_input_token)};

			// As long as the next input token is anything other than a <semicolon-token> or <EOF-token>, consume a component value and append it to the temporary list
			while (current_input_token.type() != SEMICOLON && !current_input_token.is_eof())
				temp_list.push_back(consume_component_value());

			// Consume a declaration from the temporary list. If anything was returned, append it to the list of declarations
			Parser parser(temp_list);
			if (auto declaration = parser.consume_declaration())
				declarations.push_back(*declaration);

			continue;
		}

		// anything else
		// This is a parse error
		// Reconsume the current input token
		reconsume_current_input_token();

		// As long as the next input token is anything other than a <semicolon-token> or <EOF-token>, consume a component value and throw away the returned value
		while (next_input_token().type() != SEMICOLON && !next_input_token().is_eof())
			consume_component_value();
	}
}

// 5.4.6. Consume a declaration
// https://www.w3.org/TR/css-syntax-3/#consume-declaration
std::optional<Declaration> Parser::consume_declaration()
{
	// Consume the next input token.
	consume_next_input_token();

	// Create a new declaration with its name set to the value of the current input token and its value initially set to an empty list
	auto declaration = Declaration{};
	declaration.name = current_input_token.value();

	// 1. While the next input token is a <whitespace-token>, consume the next input token
	while (next_input_token().is_whitespace())
		consume_next_input_token();

	// 2. If the next input token is anything other than a <colon-token>, this is a parse error. Return nothing
	if (next_input_token().type() != COLON)
	{
		return {};
	}

	// Otherwise, consume the next input token.
	consume_next_input_token();

	// 3. While the next input token is a <whitespace-token>, consume the next input token
	while (next_input_token().is_whitespace())
		consume_next_input_token();

	// 4. As long as the next input token is anything other than an <EOF-token>, consume a component value and append it to the declaration's value.
	while (!next_input_token().is_eof())
		declaration.value.push_back(consume_component_value());

	// 5. If the last two non-<whitespace-token>s in the declaration's value are a <delim-token> with the value "!"
	// followed by an <ident-token> with a value that is an ASCII case-insensitive match for "important",
	// remove them from the declaration's value and set the declaration's important flag to true.

	// 6. While the last token in the declaration's value is a <whitespace-token>, remove that token.

	// 7. Return the declaration.
	return declaration;
}

// 5.4.7. Consume a component value
// https://www.w3.org/TR/css-syntax-3/#consume-a-component-value
ComponentValue Parser::consume_component_value()
{
	// TODO - implement correctly
	consume_next_input_token();
	return ComponentValue(current_input_token);
}

// 5.4.8. Consume a simple block
// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
ParserBlock Parser::consume_simple_block()
{
	// Create a simple block with its associated token set to the current input token and with its value initially set to an empty list.
	ParserBlock block;

	// Repeatedly consume the next input token and process it as follows:
	while (1)
	{
		consume_next_input_token();

		// ending token
		if (current_input_token.type() == CLOSE_CURLY)
		{
			// Return the block
			return block;
		}

		// <EOF-token>
		if (!current_input_token)
		{
			// This is a parse error. Return the block
			return block;
		}

		// anything else
		// Reconsume the current input token
		reconsume_current_input_token();

		// Consume a component value and append it to the value of the block
		block.value.push_back(consume_component_value());
	}
}

std::optional<Selector> Parser::parse_complex_selector_list()
{
	consume_next_input_token();
	std::vector<Selector::ComplexSelector> complex_selector_list;
	while (auto selector = parse_complex_selector())
	{
		complex_selector_list.push_back(*selector);
		skip_whitespace();
		if (current_input_token.type() == COMMA)
		{
			consume_next_input_token();
			skip_whitespace();
		}
	}

	if (complex_selector_list.empty())
		return {};

	return Selector{complex_selector_list};
}

std::optional<Selector::ComplexSelector> Parser::parse_complex_selector()
{
	std::vector<Selector::CompoundSelector> compound_selectors;
	auto compound_selector = parse_compound_selector();
	if (!compound_selector)
		return {};

	compound_selectors.push_back(Selector::CompoundSelector{*compound_selector});

	while (true)
	{
		auto combinator = parse_combinator();
		if (!combinator)
			break;

		compound_selector = parse_compound_selector();
		if (!compound_selector)
			break;

		compound_selector->combinator = *combinator;
		compound_selectors.push_back(*compound_selector);
	}

	return Selector::ComplexSelector{compound_selectors};
}

std::optional<Selector> Parser::parse_compound_selector_list()
{
	return {};
}

std::optional<Selector::CompoundSelector> Parser::parse_compound_selector()
{
	std::vector<Selector::SimpleSelector> simple_selectors;
	while (auto selector = parse_simple_selector())
		simple_selectors.push_back(*selector);

	if (simple_selectors.empty())
		return {};
	return Selector::CompoundSelector{simple_selectors};
}

std::optional<Selector::SimpleSelector> Parser::parse_simple_selector()
{
	if (current_input_token.is_eof())
		return {};

	if (current_input_token.type() == IDENT)
	{
		auto selector = Selector::SimpleSelector{Selector::SimpleSelector::Type::Type, current_input_token.value()};
		consume_next_input_token();
		return selector;
	}

	else if (current_input_token.type() == DELIM)
	{
		if (current_input_token.value() == ".")
		{
			consume_next_input_token();
			assert(!current_input_token.is_eof());
			auto selector =
			    Selector::SimpleSelector{Selector::SimpleSelector::Type::Class, current_input_token.value()};
			consume_next_input_token();
			return selector;
		}

		else if (current_input_token.value() == "*")
		{
			auto selector =
			    Selector::SimpleSelector{Selector::SimpleSelector::Type::Universal, current_input_token.value()};
			consume_next_input_token();
			return selector;
		}

		else
		{
			fmt::print(stderr, "[SimpleSelector]: Unknown token type {}\n", current_input_token.value());
			return {};
		}
	}

	else if (current_input_token.type() == HASH)
	{
		auto selector = Selector::SimpleSelector{Selector::SimpleSelector::Type::Id, current_input_token.value()};
		consume_next_input_token();
		return selector;
	}

	return {};
}

std::optional<Selector::Combinator> Parser::parse_combinator()
{
	if (current_input_token.is_whitespace())
	{
		consume_next_input_token();
		return Selector::Combinator::Descendant;
	}

	if (current_input_token.type() != DELIM)
		return {};

	if (current_input_token.value() == "*")
		return {};

	if (current_input_token.value() == ">")
	{
		consume_next_input_token();
		return Selector::Combinator::Child;
	}

	else if (current_input_token.value() == "+")
	{
		consume_next_input_token();
		return Selector::Combinator::AdjacentSibling;
	}

	else if (current_input_token.value() == "~")
	{
		consume_next_input_token();
		return Selector::Combinator::GeneralSibling;
	}

	else
	{
		fmt::print(stderr, "Unknown combinator {}\n", current_input_token.value());
		assert(!"Unknown combinator");
	}

	return {};
}

// https://www.w3.org/TR/css-syntax-3/#normalize-into-a-token-stream
std::vector<Token> Parser::normalize(const std::string &input)
{
	Scanner scanner(input.c_str());
	tokens = scanner.tokenize();
	pos = tokens.begin();
	return tokens;
}

Token Parser::next_input_token()
{
	if (pos == tokens.end())
		return {};

	if (pos == --tokens.end())
		return {};

	return *pos;
}

void Parser::consume_next_input_token()
{
	auto next = next_input_token();
	if (!next)
	{
		current_input_token = {};
		return;
	}

	current_input_token = *pos++;
}

void Parser::reconsume_current_input_token()
{
	if (pos == tokens.begin())
		return;

	current_input_token = *pos--;
}

void Parser::skip_whitespace()
{
	while (!current_input_token.is_eof() && current_input_token.is_whitespace())
		consume_next_input_token();
}
}
