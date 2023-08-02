#pragma once

#include <string>

namespace js
{
class Cell
{
public:
	virtual ~Cell(){};

	virtual std::string to_string() const = 0;
	virtual bool is_object() const { return false; }

	bool marked = false;
	Cell *next = nullptr;
};
}
