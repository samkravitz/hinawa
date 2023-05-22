#include "browser.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "css.h"
#include "document/element.h"
#include "document/text.h"
#include "html/parser.h"
#include "layout/block.h"
#include "layout/image.h"
#include "layout/node.h"
#include "web/resource.h"

auto font = sf::Font{};
fs::path DATA_DIR = HINAWA_DATA_DIR;

// #define DEBUG_DRAW_OUTLINE

namespace browser
{
Browser::Browser(const std::string &url_string) :
    Browser(Url(url_string))
{ }

Browser::Browser(const Url &u) :
    url(u)
{
	if (!font.loadFromFile(DATA_DIR / "fonts" / "arial.ttf"))
		exit(2);

	assert(arrow_cursor.loadFromSystem(sf::Cursor::Arrow));
	assert(hand_cursor.loadFromSystem(sf::Cursor::Hand));

	css::read_properties_file();

	load(url);

	auto event = sf::Event{};

	layout::Box viewport;
	viewport.content.width = width;
	viewport.content.height = 0;
	layout_tree->layout(viewport);
	layout::print_tree_with_lines(layout_tree.get());

	while (window.isOpen())
	{
		if (document.needs_reflow())
		{
			viewport.content.height = 0;
			layout_tree->layout(viewport);
			layout_tree->print("Layout Tree");
			render();
			document.set_needs_reflow(false);
			continue;
		}

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
					window.setView(sf::View(sf::FloatRect{0, 0, (float) width, (float) height}));
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
					auto result = layout_tree->hit_test(Point{event.mouseMove.x, event.mouseMove.y});
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
	::load(url, [&](const auto &data) {
		document = Document(url);
		auto parser = html::Parser(document);
		document = parser.parse(std::string((const char *) data.data(), data.size()));
		document.print("Document");

		style_tree = css::build_style_tree(document);
		layout_tree = layout::build_layout_tree(style_tree.get());
	});
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
	auto bg = sf::RectangleShape{sf::Vector2f(width, height)};
	auto bg_color = sf::Color::White;

	if (auto *c = layout_tree->property("background"))
	{
		auto *color_value = dynamic_cast<css::Color *>(c);
		bg_color = sf::Color(color_value->r, color_value->g, color_value->b);
	}

	bg.setFillColor(bg_color);
	window.draw(bg);

	auto paint = [this](auto const &layout_node) {
		if (layout_node->is_text())
			return;

		auto dimensions = layout_node->dimensions();
		auto [x, y, width, height] = dimensions.content;

		if (auto *background = layout_node->property("background"))
		{
			auto *color = dynamic_cast<css::Color *>(background);
			sf::RectangleShape rect;
			rect.setPosition(x, y);
			rect.setSize(sf::Vector2f(width, height));
			rect.setFillColor(sf::Color(color->r, color->g, color->b));
			window.draw(rect);
		}

		if (auto *background = layout_node->property("background-color"))
		{
			auto *color = dynamic_cast<css::Color *>(background);
			if (color)
			{
				sf::RectangleShape rect;
				rect.setPosition(x, y);
				rect.setSize(sf::Vector2f(width, height));
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
					css::Color *color_value = dynamic_cast<css::Color *>(styled_node->property("color"));
					auto color = sf::Color(color_value->r, color_value->g, color_value->b);
					auto *font_size = styled_node->property("font-size");

					sf::Text text(frag.str, font);
					text.setCharacterSize(font_size->font_size());
					text.setFillColor(color);
					text.setPosition(line.x + frag.offset, line.y);
					window.draw(text);

					if (auto *decoration = styled_node->property("text-decoration"))
					{
						auto *keyword = dynamic_cast<css::Keyword *>(decoration);
						if (keyword->value == "underline")
						{
							sf::RectangleShape rect;
							rect.setPosition(line.x + frag.offset, line.y + font_size->font_size() + 1);
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
			texture.loadFromImage(image);
			sf::Sprite sprite(texture);
			sprite.setPosition(x, y);
			window.draw(sprite);
		}

		if (layout_node->is_list_item_marker())
		{
			auto marker = sf::CircleShape{width / 2.0f};
			marker.setPosition(sf::Vector2f(x, y));
			marker.setFillColor(sf::Color::Black);
			window.draw(marker);
		}
	};

	layout_tree->preorder(paint);
	window.display();
}
}
