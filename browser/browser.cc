#include "browser.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "document/element.h"
#include "document/text.h"
#include "html/parser.h"
#include "layout/block.h"
#include "layout/image.h"
#include "layout/node.h"

auto font = sf::Font{};

// #define DEBUG_DRAW_OUTLINE

namespace browser
{
Browser::Browser(const std::string &url_string)
{
	if (!font.loadFromFile("../data/fonts/arial.ttf"))
		exit(2);

	assert(arrow_cursor.loadFromSystem(sf::Cursor::Arrow));
	assert(hand_cursor.loadFromSystem(sf::Cursor::Hand));

	url = Url(url_string);
	load(url);

	auto event = sf::Event{};

	layout::Box viewport;
	viewport.content.width = width;
	viewport.content.height = 0;
	layout_tree->layout(viewport);
	layout::print_tree_with_lines(layout_tree.get());

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
					layout_tree->print("Layout Tree");
					render();
					break;
				}

				case sf::Event::MouseMoved:
				{
					auto result = layout_tree->hit_test(Point{ event.mouseMove.x, event.mouseMove.y });
					if (result && result.value()->is_link())
					{
						window.setMouseCursor(hand_cursor);

						// grabs the most specific node (Text), when we want the Element to see the href
						auto *element = dynamic_cast<Element *>(result.value()->parent());
						auto href = element->get_attribute("href");
						hovered_href = href;
					}

					else
					{
						window.setMouseCursor(arrow_cursor);
						hovered_href = "";
					}
					break;
				}

				case sf::Event::MouseButtonPressed:
				{
					if (!hovered_href.empty())
					{
						auto new_url = Url(hovered_href, &url);
						load(new_url);
						layout_tree->layout(viewport);
						render();
						break;
					}
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
	std::cout << "Loading url: " << url.path_str() << "\n";
	std::cout << url.to_string() << "\n";
	if (url.scheme() != "file")
		std::cout << "Error: unsupported scheme\n";

	std::ifstream file(url.path_str());
	std::stringstream buffer;
	buffer << file.rdbuf();

	auto parser = html::Parser(buffer.str());
	document = parser.parse();
	document.print("Document");

	style_tree = css::build_style_tree(document);
	layout_tree = layout::build_layout_tree(style_tree.get());
}

void Browser::render()
{
	window.clear();

	/**
	 * Before rendering, paint the entire canvas white.
	 * Here we will also check for an edge condition, where the background property of the <body>
	 * element is set. If it is, instead of just covering the dimensions that the body element
	 * takes up, the entire canvas is to be covered with that background color
	 * 
	 * @ref https://www.w3.org/TR/css-backgrounds-3/#special-backgrounds
	*/
	auto bg = sf::RectangleShape{ sf::Vector2f(width, height) };
	auto bg_color = sf::Color::White;

	if (auto *c = layout_tree->node()->lookup("background"))
	{
		auto *color_value = dynamic_cast<css::Color *>(c);
		bg_color = sf::Color(color_value->r, color_value->g, color_value->b);
	}

	bg.setFillColor(bg_color);
	window.draw(bg);

	auto paint = [this](auto const &layout_node) {
		auto *style = layout_node->node();
		auto dimensions = layout_node->dimensions();

		auto x = dimensions.content.x;
		auto y = dimensions.content.y;

		if (!layout_node->is_anonymous())
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

		if (layout_node->is_block())
		{
#ifdef DEBUG_DRAW_OUTLINE
			sf::RectangleShape r(sf::Vector2f(dimensions.margin_box().width, dimensions.margin_box().height));
			r.setPosition(sf::Vector2f(x, y));
			r.setOutlineThickness(2);
			r.setOutlineColor(sf::Color::Blue);
			window.draw(r);
#endif

			auto *block = static_cast<layout::Block *>(layout_node);
			for (auto const &line : block->lines)
			{
				for (auto const &frag : line.fragments)
				{
					auto *styled_node = frag.styled_node;
					css::Color *color_value = dynamic_cast<css::Color *>(styled_node->lookup("color"));
					auto color = sf::Color(color_value->r, color_value->g, color_value->b);
					auto *font_size = dynamic_cast<css::Length *>(styled_node->lookup("font-size"));

					sf::Text text(frag.str, font);
					text.setCharacterSize(font_size->to_px());
					text.setFillColor(color);
					text.setPosition(line.x + frag.offset, line.y);
					window.draw(text);

					if (auto *decoration = styled_node->lookup("text-decoration"))
					{
						auto *keyword = dynamic_cast<css::Keyword *>(decoration);
						if (keyword->value == "underline")
						{
							sf::RectangleShape rect;
							rect.setPosition(line.x + frag.offset, line.y + font_size->to_px() + 1);
							rect.setSize(sf::Vector2f(frag.len, 2));
							rect.setFillColor(color);
							window.draw(rect);
						}
					}
				}
			}
		}

		if (layout_node->is_image())
		{
			auto *image_node = static_cast<layout::Image *>(layout_node);
			auto image = image_node->image_element()->image();
			auto texture = sf::Texture{};
			texture.loadFromImage(image, sf::IntRect(x, y, dimensions.content.width, dimensions.content.height));
			window.draw(sf::Sprite(texture));
		}
	};

	layout_tree->preorder(paint);
	window.display();
}
}
