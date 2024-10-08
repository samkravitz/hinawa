#pragma once

#include "node.h"

namespace layout
{
class Text;

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
	Text *text_node;
};

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

class Block : public Node
{
public:
	Block(css::StyledNode *);
	Block();

	void layout(Box);
	std::optional<::Node *> hit_test(const Point &) override;

	std::vector<Line> lines;

	bool is_block() const override { return true; }
	bool is_anonymous() const override { return m_anonymous; }

	void render(gfx::Painter &painter) const override;

	std::string to_string() const;

private:
	bool m_anonymous = true;
	void calculate_width(Box);
	void calculate_position(Box);
	void calculate_height(Box);

	void render_text(gfx::Painter &painter) const;
};
}
