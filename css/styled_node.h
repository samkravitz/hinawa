#pragma once

#include <memory>
#include <string>
#include <vector>

#include "stylesheet.h"
#include "value.h"
#include "../document/node.h"
#include "../util/tree_node.h"

namespace css
{
class StyledNode : public util::TreeNode<StyledNode>
{
public:
	StyledNode(std::shared_ptr<Node>);
	StyledNode(std::shared_ptr<Node>, std::shared_ptr<Stylesheet>);

	inline std::shared_ptr<Node> node() const { return m_node; }
	inline std::unordered_map<std::string, Value *> values() const { return m_values; }

	Value *lookup(std::string property_name, Value * const fallback = nullptr);

private:
	// pointer to the DOM node being styled
	std::shared_ptr<Node> m_node;
	std::unordered_map<std::string, Value *> m_values;
};
}
