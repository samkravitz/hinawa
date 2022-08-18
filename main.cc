#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "css/stylesheet.h"
#include "css/styled_node.h"
#include "html/parser.h"
#include "render/window.h"
#include "layout/layout_node.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: ./hinawa [url]\n";
		return 1;
	}

	std::ifstream file(argv[1]);
	std::stringstream buffer;
	buffer << file.rdbuf();

	auto parser = html::Parser(buffer.str());
	auto document = parser.parse();
	document->print();

	auto stylesheet = css::read_default_stylesheet();
	auto style_tree = std::make_shared<css::StyledNode>(document, stylesheet);

	auto html = style_tree->last_child();
	auto body = html->last_child();
	auto layout_tree = std::make_shared<layout::LayoutNode>(body);

	Window window(layout_tree);
	return 0;
}
