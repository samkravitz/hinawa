#include "window.h"

#include <unordered_map>

#include "../document/text.h"

Window::Window(std::shared_ptr<css::StyledNode> style_tree)
{
	auto font = sf::Font{};
	auto event = sf::Event{};
	auto bg = sf::RectangleShape{ sf::Vector2f(width, height) };
	std::unordered_map<std::string, std::string> current_rules;
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

		auto draw_fn = [this, &font, &current_rules](std::shared_ptr<css::StyledNode> styled_node)
		{
			auto node = styled_node->node();
			if (node->type() == NodeType::Text)
			{
				auto text_element = std::dynamic_pointer_cast<Text>(node);
				if (!isprint(text_element->text()[0]))
					return;
				
				auto color = sf::Color::Black;
				
				if (current_rules.find("color") != current_rules.end())
				{
					auto c = current_rules["color"];

					if (c == "red")
						color = sf::Color::Red;
					
					if (c == "blue")
						color = sf::Color::Blue;
					
					if (c == "green")
						color = sf::Color::Green;
				}

				sf::Text text(text_element->text(), font);
				text.setCharacterSize(TEXT_SIZE);
				text.setFillColor(color);
				text.setPosition(0, y);
				y += TEXT_SIZE;
				window.draw(text);
			}

			else
			{
				current_rules = styled_node->values();
			}
		};

		style_tree->preorder(draw_fn);
		window.display();
	}
}
