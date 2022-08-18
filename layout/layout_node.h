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

private:
	// pointer to the styled node to be rendered
	std::shared_ptr<css::StyledNode> m_node;

	Box dimensions;
};
}
