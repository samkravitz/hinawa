#include "browser.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "document/element.h"
#include "document/text.h"
#include "html/parser.h"
#include "layout/node.h"

auto font = sf::Font{};

// #define DEBUG_DRAW_OUTLINE

namespace browser
{
Browser::Browser(const std::string &url_string)
{
	if (!font.loadFromFile("../data/fonts/arial.ttf"))
		exit(2);

	url = Url(url_string);
	load(url);

	auto event = sf::Event{};

	layout::Box viewport;
	viewport.content.width = width;
	viewport.content.height = 0;
	layout_tree->layout(viewport);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::Resized:
				{
					width = event.size.width;
					height = event.size.height;
					window.setView(sf::View(sf::FloatRect{ 0, 0, (float) width, (float) height }));
					viewport = layout::Box{};
					viewport.content.width = width;
					viewport.content.height = 0;
					layout_tree->layout(viewport);
					render();
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

void Browser::load(const Url &new_url)
{
	url = new_url;
	std::cout << "Loading url " << url.to_string() << "\n";
	if (url.scheme() != "file")
		std::cout << "Error: unsupported scheme\n";

	std::ifstream file(url.path_str());
	std::stringstream buffer;
	buffer << file.rdbuf();

	auto parser = html::Parser(buffer.str());
	document = parser.parse();
	document.print("Document");

	style_tree = css::build_style_tree(document);
	layout_tree = std::make_shared<layout::LayoutNode>(style_tree.get());
	auto layout2 = layout::build_layout_tree(style_tree.get());
	layout_tree->print("Layout Tree");
	layout2->print("Layout Tree 2.0");
}

void Browser::render()
{
	window.clear();
	auto bg = sf::RectangleShape{ sf::Vector2f(width, height) };
	bg.setFillColor(sf::Color::White);
	window.draw(bg);

	auto paint = [this](auto const &layout_node)
	{
		auto style = layout_node->node();
		auto dimensions = layout_node->dimensions();

		auto x = dimensions.content.x;
		auto y = dimensions.content.y;

#ifdef DEBUG_DRAW_OUTLINE
		sf::RectangleShape r(sf::Vector2f(dimensions.content.width, dimensions.content.height));
		r.setPosition(sf::Vector2f(x, y));
		r.setOutlineThickness(2);
		r.setOutlineColor(sf::Color::Blue);
		window.draw(r);
#endif

		if (layout_node->box_type() != layout::ANONYMOUS)
		{
			if (auto *background = style->lookup("background"))
			{
				auto *color = dynamic_cast<css::Color *>(background);
				sf::RectangleShape rect;
				rect.setPosition(x, y);
				rect.setSize(sf::Vector2f(dimensions.content.width, dimensions.content.height));
				rect.setFillColor(sf::Color(color->r, color->g, color->b));
				window.draw(rect);
			}
		}

		for (auto const &line : layout_node->lines)
		{
			for (auto const &frag : line.fragments)
			{
				auto *styled_node = frag.styled_node;
				auto text_element = dynamic_cast<Text *>(styled_node->node());
				bool is_link = text_element->is_link();
				auto *color_value = dynamic_cast<css::Color *>(style->lookup("color"));
				auto color = is_link ? sf::Color::Blue : sf::Color(color_value->r, color_value->g, color_value->b);
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
}
