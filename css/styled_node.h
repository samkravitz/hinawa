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

	inline void preorder(std::function<void(std::shared_ptr<StyledNode> node)> f)
	{
		for (auto child : children)
		{
			f(child);
			child->preorder(f);
		}
	}

	// pointer to the DOM node being styled
	std::shared_ptr<Node> node;
	std::unordered_map<std::string, std::string> values;
	std::vector<std::shared_ptr<StyledNode>> children;
};
}
