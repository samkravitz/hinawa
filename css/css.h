#pragma once

#include "stylesheet.h"

namespace css
{
struct Property
{
	bool inherited{false};
	std::string initial_value;
	int max_values{1};
	std::vector<std::string> valid_types;
};

Stylesheet read_default_stylesheet();
void read_properties_file();
Value *initial_value(const std::string &);
}
