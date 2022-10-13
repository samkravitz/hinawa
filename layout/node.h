#pragma once

#include "../css/styled_node.h"
#include "../util/tree_node.h"
#include "box.h"

namespace layout
{
class Node : public util::TreeNode<Node>
{
public:
	Node();
	Node(std::shared_ptr<css::StyledNode>);

	inline std::shared_ptr<css::StyledNode> node() const { return m_node; }
	inline Box dimensions() const { return m_dimensions; }
	inline void reset() { m_dimensions = Box{}; }

	virtual void layout(Box) = 0;
	virtual void calculate_width(Box) = 0;
	virtual void calculate_position(Box) = 0;
	virtual void calculate_height(Box) = 0;

	virtual std::string to_string() const = 0;

protected:
	// pointer to the styled node to be rendered
	std::shared_ptr<css::StyledNode> m_node;

	Box m_dimensions;
};

std::shared_ptr<Node> build_tree(std::shared_ptr<css::StyledNode>);
}
