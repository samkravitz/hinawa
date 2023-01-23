#include "parser.h"

#include <cassert>
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
	auto *value = parse_value();
	consume(SEMICOLON, "expected ;");

	auto declaration = Declaration{};
	declaration.name = name;
	declaration.value = value;
	return declaration;
}

Value *Parser::parse_value()
{
	Value *value = nullptr;

	switch (current_token.type())
	{
		case HASH:
		{
			auto hex = current_token.value();
			auto *color = new Color();
			color->r = std::stoul(hex.substr(1, 2), nullptr, 16);
			color->g = std::stoul(hex.substr(3, 2), nullptr, 16);
			color->b = std::stoul(hex.substr(5, 2), nullptr, 16);
			value = color;
			advance();
			break;
		}

		case IDENT:
		{
			if (auto *color = Color::from_color_string(current_token.value()))
			{
				value = color;
			}

			else
			{
				auto *keyword = new Keyword();
				keyword->value = current_token.value();
				value = keyword;
			}

			advance();
			break;
		}

		case LENGTH:
		{
			auto valstr = current_token.value();
			auto *length = new Length();

			// units are always 2 characters long
			auto unitstr = valstr.substr(valstr.size() - 2);
			Length::Unit unit;

			if (unitstr == "in")
				unit = Length::IN;

			if (unitstr == "cm")
				unit = Length::CM;

			if (unitstr == "mm")
				unit = Length::MM;

			if (unitstr == "pt")
				unit = Length::PT;

			if (unitstr == "pc")
				unit = Length::PC;

			if (unitstr == "px")
				unit = Length::PX;

			length->value = std::stof(valstr.substr(0, valstr.size() - 2));
			length->unit = unit;
			value = length;

			while (current_token.type() == LENGTH)
				advance();
			break;
		}

		case PERCENTAGE:
		{
			auto percentage = current_token.value().substr(0, current_token.value().size() - 1);
			value = new Percentage(std::stod(percentage));
			advance();
			break;
		}

		default:
			assert(!"Error parsing a CSS value");
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
