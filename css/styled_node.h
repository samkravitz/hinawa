#pragma once

#include <memory>
#include <string>
#include <vector>

#include "stylesheet.h"
#include "../document/node.h"

namespace css
{
class StyledNode
{
public:
	StyledNode(std::shared_ptr<Node>);
	StyledNode(std::shared_ptr<Node>, std::shared_ptr<Stylesheet>);

	inline void in_order(std::function<void(std::shared_ptr<StyledNode> node)> f)
	{
		for (auto child : children)
		{
			child->in_order(f);
			f(child);
		}
	}

	inline std::unordered_map<std::string, std::string> values() { return m_values; }

private:
	// pointer to the DOM node being styled
	std::shared_ptr<Node> node;
	std::unordered_map<std::string, std::string> m_values;
	std::vector<std::shared_ptr<StyledNode>> children;
};
}
