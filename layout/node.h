#pragma once

#include <optional>

#include "box.h"
#include "css/styled_node.h"
#include "util/tree_node.h"

namespace layout
{
class Node : public util::TreeNode<Node>
{
public:
	Node();
	Node(css::StyledNode *);

	void insert_anonymous_container();
	std::optional<::Node *> hit_test(const Point &);
	std::string tag_name() const;

	inline css::StyledNode *node() const { return m_node; }
	inline Box dimensions() const { return m_dimensions; }
	inline void reset() { m_dimensions = Box{}; }
	inline void set_block_format_context() { m_inline_format_context = false; }

	virtual void layout(Box) = 0;
	virtual void calculate_width(Box) = 0;
	virtual void calculate_position(Box) = 0;
	virtual void calculate_height(Box) = 0;
	virtual bool is_inline() const { return false; }
	virtual bool is_text() const { return false; }
	virtual bool is_anonymous() const { return false; }
	virtual bool is_block() const { return false; }

	virtual std::string to_string() const = 0;

protected:
	// pointer to the styled node to be rendered
	css::StyledNode *m_node;

	Box m_dimensions;

	bool m_inline_format_context = true;
};

std::shared_ptr<Node> build_layout_tree(css::StyledNode *);
void print_tree_with_lines(Node *);
}
