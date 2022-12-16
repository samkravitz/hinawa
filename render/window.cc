#include "window.h"

#include <unordered_map>

#include "document/element.h"
#include "document/text.h"
#include "layout/box.h"
#include "util/hinawa.h"

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
			switch (event.type)
			{
				case sf::Event::Closed: window.close(); break;

				case sf::Event::Resized:
				{
					width = event.size.width;
					height = event.size.height;
					window.setView(sf::View(sf::FloatRect{ 0, 0, (float) width, (float) height }));
					viewport = layout::Box{};
					viewport.content.width = width;
					viewport.content.height = 0;
					layout_tree->layout(viewport);
					bg = sf::RectangleShape{ sf::Vector2f(width, height) };
					window.clear();
					window.draw(bg);
					render(layout_tree);
					break;
				}

				case sf::Event::MouseMoved:
				{
					auto result = layout_tree->hit_test(Point{ event.mouseMove.x, event.mouseMove.y });
					if (result.has_value() && result.value()->is_link())
					{
						auto cursor = sf::Cursor{};
						cursor.loadFromSystem(sf::Cursor::Hand);
						window.setMouseCursor(cursor);

						// grabs the most specific node (Text), when we want the Element to see the href
						auto *element = dynamic_cast<Element *>(result.value()->parent());
						auto href = element->get_attribute("href");
						std::cout << "Hover over link! href: " << href << "\n";
					}

					else
					{
						auto cursor = sf::Cursor{};
						cursor.loadFromSystem(sf::Cursor::Arrow);
						window.setMouseCursor(cursor);
					}
					break;
				}
				default:
					break;
			}
		}
	}
}

void Window::render(const std::shared_ptr<layout::LayoutNode> &layout_tree)
{
	auto paint = [this](auto const &layout_node)
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

		for (auto const &line : layout_node->lines)
		{
			for (auto const &frag : line.fragments)
			{
				auto *styled_node = frag.styled_node;
				auto text_element = dynamic_cast<Text *>(styled_node->node());
				bool is_link = text_element->is_link();
				auto color = is_link ? sf::Color::Blue : sf::Color::Black;
				auto *font_size = dynamic_cast<css::Length *>(styled_node->lookup("font-size"));

				sf::Text text(frag.str, font);
				text.setCharacterSize(font_size->to_px());
				text.setFillColor(color);
				text.setPosition(line.x + frag.offset, line.y);
				window.draw(text);

				if (is_link)
				{
					sf::RectangleShape rect;
					rect.setPosition(line.x + frag.offset, line.y + font_size->to_px() + 1);
					rect.setSize(sf::Vector2f(frag.len, 2));
					rect.setFillColor(sf::Color::Blue);
					window.draw(rect);
				}
			}
		}
	};

	layout_tree->postorder(paint);
	window.display();
}
