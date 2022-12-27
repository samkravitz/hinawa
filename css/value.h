#pragma once

#include <string>

#include "util/hinawa.h"

namespace css
{
enum class ValueType
{
	Color,
	Keyword,
	Length,
	Unsupported,
};

enum class Display
{
	Block,
	Inline,
	InlineBlock,
	None,
};

struct Value
{
	virtual ~Value() = default;

	virtual std::string to_string() { return "{ Unsupported Value }"; }
	virtual ValueType type() { return ValueType::Unsupported; }

	virtual float to_px() { return 0; }
};

struct Color : public Value
{
	Color() = default;

	Color(u8 r, u8 g, u8 b) :
	    r(r),
	    g(g),
	    b(b),
	    a(0)
	{ }

	Color(u8 r, u8 g, u8 b, u8 a) :
	    r(r),
	    g(g),
	    b(b),
	    a(a)
	{ }

	u8 r;
	u8 g;
	u8 b;
	u8 a;

	static Color *from_color_string(std::string const &color)
	{
		if (color == "black")
			return new Color(0, 0, 0);
		if (color == "white")
			return new Color(0xff, 0xff, 0xff);
		if (color == "gray")
			return new Color(0x80, 0x80, 0x80);
		if (color == "silver")
			return new Color(0xc0, 0xc0, 0xc0);
		if (color == "maroon")
			return new Color(0x80, 0, 0);
		if (color == "red")
			return new Color(0xff, 0, 0);
		if (color == "purple")
			return new Color(0x80, 0, 0x80);
		if (color == "fushsia")
			return new Color(0xff, 0, 0xff);
		if (color == "green")
			return new Color(0, 0xff, 0);
		if (color == "lime")
			return new Color(0, 0xff, 0);
		if (color == "olive")
			return new Color(0x80, 0x80, 0);
		if (color == "yellow")
			return new Color(0xff, 0xff, 0);
		if (color == "navy")
			return new Color(0, 0, 0x80);
		if (color == "blue")
			return new Color(0, 0, 0xff);
		if (color == "teal")
			return new Color(0, 0x80, 0x80);
		if (color == "aqua")
			return new Color(0, 0xff, 0xff);

		return nullptr;
	}

	inline ValueType type() { return ValueType::Color; }
	std::string to_string()
	{
		return "{ Color r: " + std::to_string(r) + " g: " + std::to_string(g) + " b: " + std::to_string(b) +
		       " a: " + std::to_string(a) + " }";
	}
};

struct Keyword : public Value
{
	Keyword() = default;

	Keyword(std::string value) :
	    value(value)
	{ }

	std::string value;

	inline ValueType type() { return ValueType::Keyword; }

	std::string to_string() { return "{ Keyword: " + value + " }"; }
};

struct Length : public Value
{
	enum Unit
	{
		IN,    // inches
		CM,    // centimeters
		MM,    // millimeters
		PT,    // points          (1 pt = 1/72 in)
		PC,    // picas           (1 pc = 12 pt)
		PX,    // pixels          (1 px = .75 pt)
	};

	Length() = default;

	Length(float value, Unit unit) :
	    value(value),
	    unit(unit)
	{ }

	float value = 0.0f;
	Unit unit = PX;

	float to_px() { return value; }

	inline ValueType type() { return ValueType::Keyword; }

	std::string to_string()
	{
		std::string unitstr;
		switch (unit)
		{
			case IN: unitstr = "in"; break;
			case CM: unitstr = "cm"; break;
			case MM: unitstr = "mm"; break;
			case PT: unitstr = "pt"; break;
			case PC: unitstr = "pc"; break;
			case PX: unitstr = "px"; break;
		}

		return "{ Length: " + std::to_string(value) + " " + unitstr + " }";
	}
};
}
