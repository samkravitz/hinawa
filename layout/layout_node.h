#pragma once

#include "../css/styled_node.h"
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

class LayoutNode : public util::TreeNode<LayoutNode>
{
public:
	LayoutNode();
	LayoutNode(std::shared_ptr<css::StyledNode>);

	inline std::shared_ptr<css::StyledNode> node() const { return m_node; }
	inline Box dimensions() const { return m_dimensions; }
	inline BoxType box_type() const { return m_box_type; }

	void layout(Box);
	void layout_block(Box);
	void calculate_block_width(Box);
	void calculate_block_position(Box);
	void calculate_block_height(Box);

private:
	// pointer to the styled node to be rendered
	std::shared_ptr<css::StyledNode> m_node;

	Box m_dimensions;
	BoxType m_box_type;
};
}
