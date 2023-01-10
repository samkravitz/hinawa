#pragma once

#include "block.h"

namespace layout
{
class ListItemMarker final : public Node
{
public:
	ListItemMarker() :
	    Node(nullptr)
	{ }

	void layout(Box container) override;
	bool is_inline() const override { return true; }
	bool is_anonymous() const { return true; }
	bool is_list_item_marker() const override { return true; }

	std::string to_string() const override { return "<ListItemMarker>"; }
};

class ListItem final : public Block
{
public:
	ListItem(css::StyledNode *style);

	void layout(Box container) override;
	bool is_list_item() const override { return true; }
};
}
