#pragma once

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
};
}
