#pragma once

#include <string>
#include <vector>

#include "../css/styled_node.h"
#include "../util/hinawa.h"
#include "../util/tree_node.h"
#include "box.h"

namespace layout
{
enum BoxType
{
	BLOCK,
	INLINE,
	ANONYMOUS,
};

enum class FormatContext
{
	Block,
	Inline,
};

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

class LayoutNode : public util::TreeNode<LayoutNode>
{
public:
	LayoutNode();
	LayoutNode(std::shared_ptr<css::StyledNode>);

	inline std::shared_ptr<css::StyledNode> node() const { return m_node; }
	inline Box dimensions() const { return m_dimensions; }
	inline BoxType box_type() const { return m_box_type; }
	void reset();

	void layout(Box);
	void layout_block(Box);
	void layout_inline(Box);
	Point split_into_lines(const Box &, const Point &, LayoutNode *);

	void calculate_block_width(Box);
	void calculate_block_position(Box);
	void calculate_block_height(Box);

	std::string to_string() const;
	std::vector<Line> lines;

private:
	// pointer to the styled node to be rendered
	std::shared_ptr<css::StyledNode> m_node;

	Box m_dimensions;
	BoxType m_box_type;
	FormatContext m_format_context = FormatContext::Inline;
};
}
