#pragma once

#include <memory>
#include <string>

#include "css/styled_node.h"
#include "document/document.h"
#include "layout/node.h"
#include "layout/rect.h"
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
	Browser(const Url &);

private:
	Url url;
	Document document;
	std::shared_ptr<css::StyledNode> style_tree;
	std::shared_ptr<layout::Node> layout_tree;

	// href of the current hovered link
	std::string hovered_href = "";

	// width, height of window in px
	int width = 1600;
	int height = 1200;

	sf::RenderWindow window{sf::VideoMode(width, height), "hinawa"};
	sf::Cursor arrow_cursor, hand_cursor;
	layout::Rect alert_box;

	void load(const Url &);
	void render();
};
}
