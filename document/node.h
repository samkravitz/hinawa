#pragma once

#include <memory>
#include <string>
#include <unordered_map>

struct Node
{
protected:
	std::shared_ptr<Node> children;
};
