#pragma once

#include <vector>

#include "object.h"

namespace js
{
class Array final : public Object, public std::vector<Value>
{
public:
	Array();

	Array(std::vector<Value>);

	virtual Object* prototype() override;

	bool is_array() const override { return true; }
	std::string to_string() const override;
};

class ArrayPrototype final : public Object
{
public:
	ArrayPrototype(ArrayPrototype &other) = delete;
	void operator=(const ArrayPrototype &) = delete;
	Object* prototype() override { return ObjectPrototype::the(); }

	static ArrayPrototype* the();

private:
	ArrayPrototype();
	static ArrayPrototype* instance;
};
}
