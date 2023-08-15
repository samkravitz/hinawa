#pragma once

#include <string>

#include "object.h"

namespace js
{
class String;

class ObjectString : public Object
{
	friend class StringPrototype;

public:
	ObjectString(String &);
	ObjectString(String *);
	virtual Object *prototype() override;

private:
	String *primitive_string;
};

class StringPrototype final : public Object
{
	friend class Heap;

public:
	StringPrototype(StringPrototype &other) = delete;
	void operator=(const StringPrototype &) = delete;
	Object *prototype() override { return ObjectPrototype::the(); }

	static StringPrototype *the();

	std::string to_string() const override { return "StringPrototype{}"; }

private:
	StringPrototype();
	static StringPrototype *instance;
};
}
