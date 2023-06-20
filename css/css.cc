#include "css.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <unordered_map>

#include "parser.h"
#include "util/hinawa.h"

using json = nlohmann::json;

namespace css
{

css::Value *const default_font_size = new css::Length(16, css::Length::PX);
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

/**
* @brief gets the initial value of a css property
* @param property_name property name
*/
Value *initial_value(const std::string &property_name)
{
	if (properties.find(property_name) == properties.end())
	{
		fmt::print(stderr, "Can't get initial value for property {}\n", property_name);
		return nullptr;
	}

	auto property = properties[property_name];
	return Parser::parse_style_value(property_name, property.initial_value);
}

float Value::font_size() const
{
	if (auto *font_size_len = dynamic_cast<const Length*>(this))
		return font_size_len->value;
	
	if (auto *font_size_percent = dynamic_cast<const Percentage*>(this))
		return default_font_size->to_px() * (font_size_percent->percent / 100.f);
	
	fmt::print(stderr, "Bad font-size {}\n", to_string());
	return 0.f;
}
}