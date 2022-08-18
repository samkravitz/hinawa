#pragma once

#include <memory>

#include "../layout/layout_node.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// size of text character (px)
constexpr int TEXT_SIZE = 30;

class Window
{
public:
	Window(std::shared_ptr<layout::LayoutNode>);

private:
	// width, height of window in px
	int width = 800;
	int height = 800;

	sf::RenderWindow window{ sf::VideoMode(width, height), "hinawa" };
};
