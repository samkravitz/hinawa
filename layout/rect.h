#pragma once

#include <sstream>
#include <string>

#include "edges.h"

namespace layout
{
struct Rect
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	Rect expanded_by(Edges edges)
	{
		auto rect = Rect{};

		rect.x = x - edges.left;
		rect.y = y - edges.top;
		rect.width = width + edges.left + edges.right;
		rect.height = height + edges.top + edges.bottom;

		return rect;
	}

	std::string to_string() const
	{
		std::stringstream ss;
		ss << "{ Rect ";
		ss << "x: " << x;
		ss << " y: " << y;
		ss << " w: " << width;
		ss << " h: " << height;
		ss << " }";

		return ss.str();
	}
};
}
