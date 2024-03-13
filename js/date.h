#pragma once

#include "object.h"

namespace js
{
class Date final : public Object
{
public:
	Date();

	virtual Object *prototype() override;

	std::uint64_t time() const { return m_time; }

	bool is_date() const override { return true; }
	std::string to_string() const override;

private:
	std::uint64_t m_time;    // millis since Unix epoch
};

class DatePrototype final : public Object
{
	friend class Heap;

public:
	DatePrototype(DatePrototype &other) = delete;
	void operator=(const DatePrototype &) = delete;
	Object *prototype() override { return ObjectPrototype::the(); }

	static DatePrototype *the();

private:
	DatePrototype();
	static DatePrototype *instance;
};
}
