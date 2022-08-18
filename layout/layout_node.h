#pragma once

#include "../css/styled_node.h"
#include "../util/tree_node.h"
#include "box.h"

namespace layout
{
class LayoutNode : public util::TreeNode<LayoutNode>
{
public:
	LayoutNode() = default;
	LayoutNode(std::shared_ptr<css::StyledNode>);

	inline std::shared_ptr<css::StyledNode> node() const { return m_node; }
	inline Box dimensions() const { return m_dimensions; }

	inline Edges margin() const { return m_dimensions.margin; }

	void calculate_layout(Box);
	void calculate_width(Box);
	void calculate_position(Box);
	void calculate_height(Box);

private:
	// pointer to the styled node to be rendered
	std::shared_ptr<css::StyledNode> m_node;

	Box m_dimensions;
};
}
