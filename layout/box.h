#pragma once

#include "rect.h"

namespace layout
{
struct Edges
{
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;
};

struct Box
{
	// content box
	Rect content;

	Edges border;
	Edges margin;
	Edges padding;
};
}
