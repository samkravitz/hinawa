#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "value.h"

namespace js
{
class Object
{
public:
	virtual ~Object() { }

	Value get(std::string const &);
	void set(std::string, Value);
	void set_native(const std::string &, const std::function<Value(std::vector<Value>)> &);

	bool is_defined(std::string const &) const;
	std::string to_string() const;

protected:
	std::unordered_map<std::string, Value> properties;
	virtual Object *prototype();
};

class ObjectPrototype final : public Object
{
public:
	ObjectPrototype(ObjectPrototype &other) = delete;
	void operator=(const ObjectPrototype &) = delete;
	Object *prototype() override { return nullptr; }

	static ObjectPrototype *the();

private:
	ObjectPrototype();
	static ObjectPrototype *instance;
};
}