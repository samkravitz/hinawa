#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../document/node.h"
#include "../util/tree_node.h"
#include "stylesheet.h"
#include "value.h"

namespace css
{
class StyledNode : public util::TreeNode<StyledNode>
{
public:
	StyledNode(std::shared_ptr<Node>,
	           std::shared_ptr<Stylesheet>,
	           std::unordered_map<std::string, Value *> *parent_values = nullptr);

	inline std::shared_ptr<Node> node() const { return m_node; }
	inline std::unordered_map<std::string, Value *> values() const { return m_values; }

	Value *lookup(std::string property_name, Value *const fallback = nullptr);
	Value *lookup(std::string property_name1, std::string property_name2, Value *const fallback = nullptr);
	Display display();

private:
	// pointer to the DOM node being styled
	std::shared_ptr<Node> m_node;
	std::unordered_map<std::string, Value *> m_values;
};
}
