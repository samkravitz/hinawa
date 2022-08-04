#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class NodeType
{
	Node,
	Element,
	Text,
};

struct Node
{
public:
	Node() { }
	~Node() { }

	void add_child(std::shared_ptr<Node>);
	void in_order(std::function<void(std::shared_ptr<Node>)>);
	bool has_children();

	std::shared_ptr<Node> last_child();

	virtual NodeType type();
	virtual void print(int depth = 0);

protected:
	std::vector<std::shared_ptr<Node>> children;
};
