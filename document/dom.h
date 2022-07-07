#pragma once

#include <memory>

#include "node.h"

class Dom
{
public:
	Dom(std::shared_ptr<Node>);
	Dom();

private:
	std::shared_ptr<Node> root;
};
