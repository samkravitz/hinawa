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
			if (next_input_token().type() != SEMICOLON && !next_input_token().is_eof())
				temp_list.push_back(consume_component_value());

			// Consume a declaration from the temporary list. If anything was returned, append it to the list of declarations
			if (auto declaration = consume_declaration())
				declarations.push_back(*declaration);

			continue;
		}

		// anything else
		// This is a parse error
		// Reconsume the current input token
		reconsume_current_input_token();

		// As long as the next input token is anything other than a <semicolon-token> or <EOF-token>, consume a component value and throw away the returned value
		if (next_input_token().type() != SEMICOLON && !next_input_token().is_eof())
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
	if (!next_input_token().is_eof())
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

	return *(pos + 1);
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
}
