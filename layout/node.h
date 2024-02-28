#pragma once

#include <optional>

#include "box.h"
#include "css/styled_node.h"
#include "gfx/painter.h"
#include "gfx/point.h"
#include "util/tree_node.h"

namespace layout
{
class Node : public util::TreeNode<Node>
{
public:
	Node();
	Node(css::StyledNode *);

	void insert_anonymous_container();
	virtual std::optional<::Node *> hit_test(const Point &);
	std::string tag_name() const;
	css::Value *property(const std::string &name) const
	{
		if (is_anonymous())
		{
			css::StyledNode node;
			node.inherit_properties(*parent()->style());
			return node.property(name);
		}

		return m_style->property(name);
	}

	inline css::StyledNode *style() const { return m_style; }
	inline Box &dimensions() { return m_dimensions; }
	inline void reset() { m_dimensions = Box{}; }
	inline void set_block_format_context() { m_inline_format_context = false; }

	virtual void layout(Box) = 0;
	virtual bool is_inline() const { return false; }
	virtual bool is_text() const { return false; }
	virtual bool is_anonymous() const { return false; }
	virtual bool is_block() const { return false; }
	virtual bool is_canvas() const { return false; }
	virtual bool is_image() const { return false; }
	virtual bool is_list_item() const { return false; }
	virtual bool is_list_item_marker() const { return false; }

	virtual void render(gfx::Painter &painter) const { }

	virtual std::string to_string() const = 0;

protected:
	// pointer to the styled node to be rendered
	css::StyledNode *m_style;

	Box m_dimensions;

	bool m_inline_format_context = true;
};

std::shared_ptr<Node> build_layout_tree(css::StyledNode *);
void print_tree_with_lines(Node *);
}
