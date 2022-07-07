#pragma once

#include <string>

#include "node.h"

struct Element : public Node
{
public:
	Element(std::string);

private:
	std::string tag;
	std::unordered_map<std::string, std::string> attrs;
};
