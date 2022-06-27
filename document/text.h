#pragma once

#include <string>

#include "node.h"

struct Text : protected Node
{
private:
	std::string text;
};
