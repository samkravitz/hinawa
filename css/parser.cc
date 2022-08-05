#include "parser.h"

#include <sstream>

namespace css
{
Parser::Parser(std::string input) :
	input(input)
{
	std::istringstream istream(input);
	lexer = new FlexLexer(&istream);
}

Parser::~Parser()
{
	delete lexer;
}

std::shared_ptr<Stylesheet> Parser::parse()
{
	return parse_stylesheet();
}

std::shared_ptr<Stylesheet> Parser::parse_stylesheet()
{
	auto stylesheet = std::make_shared<Stylesheet>();
	std::shared_ptr<Rule> rule = nullptr;

	while ((rule = parse_rule()))
	{
		stylesheet->rules.push_back(*rule);
	}

	return stylesheet;
}

std::shared_ptr<Rule> Parser::parse_rule()
{
	std::shared_ptr<Rule> rule = nullptr;
	std::shared_ptr
}

std::shared_ptr<Selector> Parser::parse_selector()
{

}

std::shared_ptr<Declaration> Parser::parse_declaration()
{

}
}
