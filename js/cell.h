#pragma once

namespace js
{
class Cell
{
public:
	virtual ~Cell(){};

	bool marked = false;
	Cell *next = nullptr;
};
}
