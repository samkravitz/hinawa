#pragma once

#include <string>

#include "node.h"

struct Element : protected Node
{
private:
	std::string tag;
	std::unordered_map<std::string, std::string> attrs;
};
