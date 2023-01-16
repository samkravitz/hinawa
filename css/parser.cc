// https://www.w3.org/TR/css-syntax-3/#parsing
#include "parser.h"

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
	parser.normalize(input);

	// 3. Create a new stylesheet, with its location set to location (or null, if location was not passed).
	Stylesheet stylesheet = {};

	// 4. Consume a list of rules from input, with the top-level flag set, and set the stylesheet's value to the result
	stylesheet.rules = parser.consume_list_of_rules(true);

	// 5. Return the stylesheet
	return stylesheet;
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
				;
			
			// Otherwise, reconsume the current input token
			// Consume a qualified rule. If anything is returned, append it to the list of rules
			else
			{
				reconsume_current_input_token();
				//auto qualified_rule = consume_qualified_rule();
				//if (qualified_rule)
				//	rules.push_back(*qualified_rule);
			}
		}

		// <at-keyword_token>

		// anything else
		// Reconsume the current input token.
		// Consume a qualified rule. If anything is returned, append it to the list of rules. 
	}

	return rules;
}

// https://www.w3.org/TR/css-syntax-3/#normalize-into-a-token-stream
std::vector<Token> Parser::normalize(const std::string &input)
{
	Scanner scanner(input.c_str());
	tokens = scanner.tokenize();
	pos = tokens.begin();
	return tokens;
}

void Parser::consume_next_input_token()
{
	if (pos == tokens.end())
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
