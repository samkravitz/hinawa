#pragma once

#include <memory>
#include <string>

#include "css/styled_node.h"
#include "document/document.h"
#include "layout/layout_node.h"
#include "web/url.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace browser
{
class Browser
{
public:
	Browser() = default;
	Browser(const std::string &);

private:
	Url url;
	Document document;
	std::shared_ptr<css::StyledNode> style_tree;
	std::shared_ptr<layout::LayoutNode> layout_tree;

	// width, height of window in px
	int width = 800;
	int height = 600;

	sf::RenderWindow window{ sf::VideoMode(width, height), "hinawa" };

	void load(const Url &);
	void render();
};
}
