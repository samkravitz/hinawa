#pragma once

#include <memory>
#include <string>
#include <vector>

#include "stylesheet.h"
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
	inline std::unordered_map<std::string, std::string> values() const { return m_values; }

private:
	// pointer to the DOM node being styled
	std::shared_ptr<Node> m_node;
	std::unordered_map<std::string, std::string> m_values;
};
}
