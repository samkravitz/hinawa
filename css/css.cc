#include "css.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "parser.h"

using json = nlohmann::json;

namespace css
{
Stylesheet read_default_stylesheet()
{
	std::ifstream file("../data/default.css");
	std::stringstream buffer;
	buffer << file.rdbuf();

	return Parser::parse(buffer.str());
}

void read_properties_file()
{
	std::ifstream f("../data/css_properties.json");
	auto data = json::parse(f);
	std::cout << data << "\n";
}
}