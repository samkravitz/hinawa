#pragma once

#include <string>
#include <vector>

namespace css
{
struct Rule
{
	std::vector<Selector> selectors;
	std::vector<Declaration> declarations;
};

// ex
// div
// p, h1
struct Selector
{
	std::string tag_name;
	std::string id;
	std::string class_name;
};


// ex
// margin: auto
struct Declaration
{
	std::string name;
	std::string value;
};

struct Stylesheet
{
	Stylesheet() = default;

	std::vector<Rule> rules;
};
}
