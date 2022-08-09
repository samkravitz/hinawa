#include "stylesheet.h"

#include <fstream>
#include <sstream>

#include "parser.h"

namespace css
{
std::shared_ptr<Stylesheet> read_default_stylesheet()
{
	std::ifstream file("css/default.css");
	std::stringstream buffer;
	buffer << file.rdbuf();

	auto parser = Parser(buffer.str());
	return parser.parse();
}
}
