#pragma once

#include <string>

#include "../util/hinawa.h"

namespace css
{
enum class ValueType
{
	Color,
	Keyword,
	Unsupported,
};

struct Value
{
	virtual ~Value() = default;

	virtual std::string to_string() { return "{ Unsupported Value }"; }
	virtual ValueType type() { return ValueType::Unsupported; }
};

struct Color : public Value
{
	u8 r;
	u8 g;
	u8 b;
	u8 a;

	inline ValueType type() { return ValueType::Color; }
	std::string to_string()
	{
		return "{ Color r: " + std::to_string(r) + " g: " + std::to_string(g) + " b: " + std::to_string(b) + " a: " + std::to_string(a) + " }";
	}
};

struct Keyword : public Value
{
	std::string value;

	inline ValueType type() { return ValueType::Keyword; }

	std::string to_string() { return "{ Keyword: " + value + " }"; }
};
}
