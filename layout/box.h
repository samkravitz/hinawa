#pragma once

#include <sstream>
#include <string>

#include "edges.h"
#include "rect.h"

namespace layout
{
struct Box
{
	// content box
	Rect content;

	Edges border;
	Edges margin;
	Edges padding;

	Rect padding_box() { return content.expanded_by(padding); }
	Rect border_box() { return padding_box().expanded_by(border); }
	Rect margin_box() { return border_box().expanded_by(margin); }

	std::string to_string() const
	{
		std::stringstream ss;
		ss << "content: " << content.to_string();
		ss << " margin: " << margin.to_string();
		ss << " border: " << border.to_string();
		ss << " padding: " << padding.to_string();

		return ss.str();
	}
};
}
