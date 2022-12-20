#pragma once

#include "node.h"
#include "util/hinawa.h"

namespace layout
{
class Block : public Node
{
public:
	Block(css::StyledNode *);
	Block();

	struct LineFragment
	{
		LineFragment()
		{
			str = "";
			offset = 0;
			len = 0;
			styled_node = nullptr;
		}

		std::string str;

		// horizontal offset in pixels from the first element in the line
		int offset;

		// length in pixels of the line fragment
		int len;

		css::StyledNode *styled_node;
	};

	void layout(Box);
	std::string to_string() const;

	struct Line
	{
		Line(int x, int y) :
		    x(x),
		    y(y)
		{ }

		int x;
		int y;
		int height = 0;
		int width = 0;
		std::vector<LineFragment> fragments;
	};

	std::vector<Line> lines;

	bool is_block() const { return true; }
	bool is_anonymous() const { return m_anonymous; }

private:
	bool m_anonymous = true;
	void calculate_width(Box);
	void calculate_position(Box);
	void calculate_height(Box);

	Point split_into_lines(const Box &container_start, const Point &offset, Node *);
};
}
