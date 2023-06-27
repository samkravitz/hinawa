#pragma once

#include "css/value.h"
#include "hinawa.h"

struct Color
{
	Color() = default;
	Color(const css::Color &c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
	}

	Color(u8 r, u8 g, u8 b) :
	    r(r),
	    g(g),
	    b(b),
	    a(0xff)
	{ }

	Color(u8 r, u8 g, u8 b, u8 a) :
	    r(r),
	    g(g),
	    b(b),
	    a(a)
	{ }

	u32 to_u32() const { return (a << 24) | (r << 16) | (g << 8) | b; }

	u8 r;
	u8 g;
	u8 b;
	u8 a = 0xff;
};