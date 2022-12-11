#pragma once

#include <string>

#include "object.h"

namespace js
{
class String : public Object
{
public:
	String(const std::string &);
	virtual Object *prototype() override;

private:
	std::string mstring;
};

class StringPrototype final : public Object
{
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
