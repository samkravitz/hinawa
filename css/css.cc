#include "css.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "parser.h"
#include "util/hinawa.h"

using json = nlohmann::json;

namespace css
{

std::unordered_map<std::string, Property> properties;

Stylesheet read_default_stylesheet()
{
	std::ifstream file(DATA_DIR / "default.css");
	std::stringstream buffer;
	buffer << file.rdbuf();

	return Parser::parse_stylesheet(buffer.str());
}

void read_properties_file()
{
	std::ifstream f(DATA_DIR / "css_properties.json");
	auto data = json::parse(f);
	for (auto &[key, value] : data.items())
	{
		Property property{};
		if (value.contains("inherited"))
			property.inherited = value["inherited"].get<bool>();

		if (value.contains("initial_value"))
			property.initial_value = value["initial_value"].get<std::string>();

		if (value.contains("max_values"))
			property.max_values = value["max_values"].get<int>();

		if (value.contains("valid_types"))
			property.valid_types = value["valid_types"].get<std::vector<std::string>>();
		
		properties[key] = property;
	}
}
}