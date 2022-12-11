#include "window.h"

#include <unordered_map>

#include "../document/text.h"
#include "../layout/box.h"

// #define DEBUG_DRAW_OUTLINE

auto font = sf::Font{};

Window::Window(std::shared_ptr<layout::LayoutNode> layout_tree)
{
	auto event = sf::Event{};
	auto bg = sf::RectangleShape{ sf::Vector2f(width, height) };
	bg.setFillColor(sf::Color::White);

	layout::Box viewport;
	viewport.content.width = width;
	viewport.content.height = 0;

	if (!font.loadFromFile("../data/fonts/arial.ttf"))
		exit(2);

	layout_tree->layout(viewport);
	layout_tree->print("Layout Tree");

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::Resized)
			{
				width = event.size.width;
				height = event.size.height;
				window.setView(sf::View(sf::FloatRect{ 0, 0, (float) width, (float) height }));
				viewport = layout::Box{};
				viewport.content.width = width;
				viewport.content.height = 0;
				layout_tree->layout(viewport);
				bg = sf::RectangleShape{ sf::Vector2f(width, height) };
			}
		}

		window.clear();
		window.draw(bg);

		auto draw_fn = [this](std::shared_ptr<layout::LayoutNode> layout_node)
		{
			// anonymous boxes don't get drawn
			if (layout_node->box_type() == layout::ANONYMOUS)
				return;

			auto style = layout_node->node();
			auto dimensions = layout_node->dimensions();
			auto *background = style->lookup("background");

			auto x = dimensions.content.x;
			auto y = dimensions.content.y;

#ifdef DEBUG_DRAW_OUTLINE
			sf::RectangleShape r(sf::Vector2f(dimensions.content.width, dimensions.content.height));
			r.setPosition(sf::Vector2f(x, y));
			r.setOutlineThickness(2);
			r.setOutlineColor(sf::Color::Blue);
			window.draw(r);
#endif

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
				bool is_link = text_element->is_link();
				auto color = is_link ? sf::Color::Blue : sf::Color::Black;
				auto *font_size = dynamic_cast<css::Length *>(style->lookup("font-size"));

				for (auto line : layout_node->lines)
				{
					for (auto item : line.items)
					{
						sf::Text text(item.str, font);
						text.setCharacterSize(font_size->to_px());
						text.setFillColor(color);
						text.setPosition(line.x + item.offset, line.y);
						window.draw(text);
					}

					if (is_link)
					{
						sf::RectangleShape rect;
						rect.setPosition(line.x, line.y + font_size->to_px() + 2);
						rect.setSize(sf::Vector2f(line.width, 2));
						rect.setFillColor(sf::Color::Blue);
						window.draw(rect);
					}
				}
			}
		};

		layout_tree->postorder(draw_fn);
		window.display();
	}
}
