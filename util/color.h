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
		a = c.b;
	}

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
};