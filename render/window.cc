#include "window.h"

#include "../document/text.h"

Window::Window(std::shared_ptr<Node> document)
{
	auto font = sf::Font{};
	auto event = sf::Event{};
	auto bg = sf::RectangleShape{ sf::Vector2f(width, height) };
	bg.setFillColor(sf::Color::White);

	if (!font.loadFromFile("data/fonts/FiraSans-Book.otf"))
		exit(2);

	while (window.isOpen())
	{
		y = 0;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(bg);

		auto draw_fn = [this, &font](std::shared_ptr<Node> node)
		{
			if (node->type() == NodeType::Text)
			{
				auto text_element = std::dynamic_pointer_cast<Text>(node);
				if (!isprint(text_element->text()[0]))
					return;

				sf::Text text(text_element->text(), font);
				text.setCharacterSize(TEXT_SIZE);
				text.setFillColor(sf::Color::Black);
				text.setPosition(0, y);
				y += TEXT_SIZE;
				window.draw(text);
			}
		};

		document->in_order(draw_fn);
		window.display();
	}
}
