#include "parser.h"

#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <sstream>

namespace css
{
Parser::Parser(std::string input) :
    scanner(input.c_str())
{
	advance();
}

Stylesheet Parser::parse(std::string input)
{
	Parser parser(input);
	return parser.parse_stylesheet();
}

auto Parser::parse_inline(std::string input) -> std::vector<Declaration>
{
	Parser parser(input);
	std::vector<Declaration> declarations;
	while (auto declaration = parser.parse_declaration())
	{
		declarations.push_back(*declaration);
		if (parser.is_eof())
			break;
	}

	return declarations;
}

Stylesheet Parser::parse_stylesheet()
{
	auto stylesheet = Stylesheet{};

	while (auto rule = parse_rule())
	{
		stylesheet.rules.push_back(*rule);
	}

	return stylesheet;
}

auto Parser::parse_rule() -> std::optional<Rule>
{
	auto rule = Rule{};
	auto selector = parse_selector();

	if (!selector)
		return {};

	rule.selectors.push_back(*selector);

	while (peek() != OPEN_BRACE)
	{
		consume(COMMA, "expected ','");
		selector = parse_selector();
		if (!selector)
			std::cout << "expected selector\n";

		rule.selectors.push_back(*selector);
	}

	if (match(OPEN_BRACE))
	{
		while (!match(CLOSE_BRACE))
		{
			auto declaration = parse_declaration();
			rule.declarations.push_back(*declaration);
		}
	}

	return rule;
}

auto Parser::parse_selector() -> std::optional<Selector>
{
	auto selector = Selector{};
	std::vector<SimpleSelector> simple_selectors;

	while (auto simple = parse_simple_selector())
		simple_selectors.push_back(*simple);

	if (simple_selectors.empty())
		return {};

	if (simple_selectors.size() == 1)
	{
		selector.type = Selector::Type::Simple;
		selector.simple_selector = simple_selectors[0];
	}

	else
	{
		CompoundSelector compound_selector;
		compound_selector.simple_selectors = simple_selectors;
		selector.type = Selector::Type::Compound;
		selector.compound_selector = compound_selector;
	}

	return selector;
}

auto Parser::parse_simple_selector() -> std::optional<SimpleSelector>
{
	// universal selector *
	if (current_token.value() == "*")
	{
		auto simple = SimpleSelector{};
		simple.type = SimpleSelector::Type::Universal;
		advance();
		return simple;
	}

	if (current_token.value() == ".")
	{
		auto simple = SimpleSelector{};
		simple.type = SimpleSelector::Type::Class;
		advance();
		simple.value = current_token.value();
		advance();
		return simple;
	}

	if (current_token.type() == HASH)
	{
		auto simple = SimpleSelector{};
		simple.type = SimpleSelector::Type::Id;
		simple.value = current_token.value().substr(1);
		advance();
		return simple;
	}

	if (current_token.type() == IDENT)
	{
		auto simple = SimpleSelector{};
		simple.type = SimpleSelector::Type::Type;
		simple.value = current_token.value();
		advance();

		if (match(COLON))
		{
			advance();
			simple.type = SimpleSelector::Type::PseudoClass;
		}

		return simple;
	}

	return {};
}

auto Parser::parse_declaration() -> std::optional<Declaration>
{
	consume(IDENT, "expected identifier");
	auto name = previous_token.value();
	consume(COLON, "expected ':'");
	auto value = parse_value();

	auto declaration = Declaration{};
	declaration.name = name;
	declaration.value = value;
	return declaration;
}

std::string Parser::parse_value()
{
	std::string value = "";
	while (!match(SEMICOLON))
	{
		if (is_eof())
		{
			fmt::print(stderr, "expected ';'\n");
			return value;
		}

		value += current_token.value();
		advance();
	}

	return value;
}

void Parser::advance()
{
	previous_token = current_token;
	current_token = scanner.next();
}

bool Parser::match(TokenType type)
{
	if (current_token.type() == type)
	{
		advance();
		return true;
	}

	return false;
}

void Parser::consume(TokenType type, const char *msg)
{
	if (!match(type))
	{
		std::cout << msg << "\n";
	}
}

TokenType Parser::peek()
{
	return current_token.type();
}

bool Parser::is_eof() const
{
	return current_token.type() == 0;
}
}
