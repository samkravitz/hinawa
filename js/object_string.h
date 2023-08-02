#pragma once

#include <string>

#include "object.h"

namespace js
{
class PrimitiveString;

class ObjectString : public Object
{
	friend class StringPrototype;

public:
	ObjectString(PrimitiveString &);
	ObjectString(PrimitiveString *);
	virtual Object *prototype() override;

private:
	PrimitiveString *primitive_string;
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
