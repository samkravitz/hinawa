#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "document/document.h"
#include "stylesheet.h"
#include "util/tree_node.h"
#include "value.h"

namespace css
{
class StyledNode : public util::TreeNode<StyledNode>
{
public:
	StyledNode() = default;
	StyledNode(Node *);

	inline Node *node() const { return m_node; }
	inline std::unordered_map<std::string, Value *> values() const { return m_values; }
	void assign(const std::string &name, Value *value);

	Value *property(const std::string &property_name) const;
	std::string string_or_fallback(const std::string &, const std::string &) const;
	Display display();
	void inherit_properties(const StyledNode &);

private:
	// pointer to the DOM node being styled
	Node *m_node;
	std::unordered_map<std::string, Value *> m_values;
};

std::shared_ptr<StyledNode> build_style_tree(const Document &document);
}
