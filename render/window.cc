#include "window.h"

#include <unordered_map>

#include "../document/text.h"
#include "../layout/box.h"

Window::Window(std::shared_ptr<layout::LayoutNode> layout_tree)
{
	auto font = sf::Font{};
	auto event = sf::Event{};
	auto bg = sf::RectangleShape{ sf::Vector2f(width, height) };
	bg.setFillColor(sf::Color::White);

	layout::Box viewport;
	viewport.content.width = width;
	viewport.content.height = 0;

	layout_tree->layout(viewport);

	if (!font.loadFromFile("../data/fonts/FiraSans-Book.otf"))
		exit(2);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(bg);

		auto draw_fn = [this, &font](std::shared_ptr<layout::LayoutNode> layout_node)
		{
			// anonymous boxes don't get drawn
			if (layout_node->box_type() == layout::ANONYMOUS)
				return;

			auto style = layout_node->node();
			auto dimensions = layout_node->dimensions();
			auto *background = style->lookup("background");

			auto x = dimensions.content.x;
			auto y = dimensions.content.y;

			if (background)
			{
				auto *color = dynamic_cast<css::Color *>(background);
				sf::RectangleShape rect;
				rect.setPosition(x, y);
				rect.setSize(sf::Vector2f(dimensions.content.width, dimensions.content.height));
				rect.setFillColor(sf::Color(color->r, color->g, color->b));
				window.draw(rect);
			}

			if (style->node()->type() == NodeType::Text)
			{
				auto text_element = std::dynamic_pointer_cast<Text>(style->node());
				auto color = sf::Color::Black;

				sf::Text text(text_element->text(), font);
				text.setCharacterSize(TEXT_SIZE);
				text.setFillColor(color);
				text.setPosition(x, y);
				window.draw(text);
			}
		};

		layout_tree->postorder(draw_fn);
		window.display();
	}
}
