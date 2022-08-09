#pragma once

#include <memory>

#include "../css/styled_node.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// size of text character (px)
constexpr int TEXT_SIZE = 30;

class Window
{
public:
	Window(std::shared_ptr<css::StyledNode>);

private:
	// width, height of window in px
	int width = 800;
	int height = 800;
	int y = 0;

	sf::RenderWindow window{ sf::VideoMode(width, height), "hinawa" };
};
