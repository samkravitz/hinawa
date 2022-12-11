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
				auto color = sf::Color::Black;
				auto *font_size = dynamic_cast<css::Length *>(style->lookup("font-size"));

				for (auto item : layout_node->line_items)
				{
					sf::Text text(item.str, font);
					text.setCharacterSize(font_size->to_px());

					if (text_element->is_link())
					{
						color = sf::Color::Blue;
						sf::RectangleShape rect;
						rect.setPosition(item.x, item.y + font_size->to_px() + 2);
						rect.setSize(sf::Vector2f(text.getLocalBounds().width, 2));
						rect.setFillColor(color);
						window.draw(rect);
					}

					text.setFillColor(color);
					text.setPosition(item.x, item.y);
					window.draw(text);
				}
			}
		};

		layout_tree->postorder(draw_fn);
		window.display();
	}
}
