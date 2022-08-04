#include "window.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

Window::Window()
{
	sf::RenderWindow window(sf::VideoMode(width, height), "hinawa");
	sf::RectangleShape bg(sf::Vector2f(width, height));
	bg.setFillColor(sf::Color::White);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(bg);
		window.display();
	}
}
