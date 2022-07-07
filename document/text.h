#pragma once

#include <string>

#include "node.h"

struct Text : public Node
{
public:
	Text(std::string);

private:
	std::string text;
};
