#pragma once

#include <unordered_map>

#include "css/value.h"

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

	static const Color BLACK;
	static const Color WHITE;

	u32 to_u32() const { return (a << 24) | (r << 16) | (g << 8) | b; }

	u8 r;
	u8 g;
	u8 b;
	u8 a = 0xff;

	static Color from_string(const std::string &color)
	{
		static std::unordered_map<std::string, Color> color_strings = {
		    {"black",   Color(0,    0,    0)   },
		    {"white",   Color(0xff, 0xff, 0xff)},
		    {"gray",    Color(0x80, 0x80, 0x80)},
		    {"silver",  Color(0xc0, 0xc0, 0xc0)},
		    {"maroon",  Color(0x80, 0,    0)   },
		    {"red",     Color(0xff, 0,    0)   },
		    {"purple",  Color(0x80, 0,    0x80)},
		    {"fushsia", Color(0xff, 0,    0xff)},
		    {"green",   Color(0,    0xff, 0)   },
		    {"lime",    Color(0,    0xff, 0)   },
		    {"olive",   Color(0x80, 0x80, 0)   },
		    {"yellow",  Color(0xff, 0xff, 0)   },
		    {"navy",    Color(0,    0,    0x80)},
		    {"blue",    Color(0,    0,    0xff)},
		    {"teal",    Color(0,    0x80, 0x80)},
		    {"aqua",    Color(0,    0xff, 0xff)},
		};

		if (color_strings.contains(color))
			return color_strings[color];

		return Color::BLACK;
	}
};

inline const Color Color::BLACK(0x0, 0x0, 0x0);
inline const Color Color::WHITE(0xff, 0xff, 0xff);
