#pragma once

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
};
}
