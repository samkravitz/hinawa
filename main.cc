#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "css/styled_node.h"
#include "css/stylesheet.h"
#include "html/parser.h"
#include "layout/layout_node.h"
#include "layout/node.h"
#include "render/window.h"

int main(int argc, char **argv)
{
	auto html_file = "../data/homepage.html";
	if (argc > 1)
		html_file = argv[1];

	std::ifstream file(html_file);
	std::stringstream buffer;
	buffer << file.rdbuf();

	auto parser = html::Parser(buffer.str());
	auto document = parser.parse();
	document.root()->print("Document");

	auto stylesheet = css::read_default_stylesheet();
	auto style_tree = std::make_shared<css::StyledNode>(document.root(), stylesheet);

	auto html = style_tree->last_child();
	auto body = html->last_child();
	auto layout_tree = std::make_shared<layout::LayoutNode>(body);

	auto tree = layout::build_tree(body);
	tree->print("Layout Tree 2.0");

	Window window(layout_tree);
	return 0;
}
