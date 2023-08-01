#pragma once

#include <string>

#include "object.h"

namespace js
{
class ObjectString : public Object
{
	friend class StringPrototype;

public:
	ObjectString(const std::string &);
	ObjectString(std::string *);
	~ObjectString();
	virtual Object *prototype() override;

private:
	std::string *primitive_string;
};

class StringPrototype final : public Object
{
	friend class Heap;

public:
	StringPrototype(StringPrototype &other) = delete;
	void operator=(const StringPrototype &) = delete;
	Object *prototype() override { return ObjectPrototype::the(); }

	static StringPrototype *the();

private:
	StringPrototype();
	static StringPrototype *instance;
};
}
