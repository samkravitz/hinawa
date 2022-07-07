#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct Node
{
public:
	virtual void add_child(std::shared_ptr<Node>);

protected:
	std::vector<std::shared_ptr<Node>> children;
};
