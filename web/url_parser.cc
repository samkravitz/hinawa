#include "url_parser.h"

#include <cctype>
#include <iostream>

#include "util/hinawa.h"

static inline bool is_ascii_digit(u32 c) { return c >= '0' && c <= '9'; }
static inline bool is_ascii_upper_hex_digit(u32 c) { return c >= 'A' && c <= 'F'; }
static inline bool is_ascii_lower_hex_digit(u32 c) { return c >= 'a' && c <= 'f'; }
static inline bool is_ascii_upper_alpha(u32 c) { return c >= 'A' && c <= 'Z'; }
static inline bool is_ascii_lower_alpha(u32 c) { return c >= 'a' && c <= 'z'; }
static inline bool is_ascii_alpha(u32 c) { return is_ascii_upper_alpha(c) || is_ascii_lower_alpha(c); }
static inline bool is_ascii_alphanumeric(u32 c) { return is_ascii_alpha(c) || is_ascii_digit(c); }

// implements the basic URL parser spec
// https://url.spec.whatwg.org/#concept-basic-url-parser
Url UrlParser::parse(std::string url_string)
{
	Url url;
	buffer = "";

	auto remaining_starts_with = [](auto const &it, std::string const &rem) -> bool
	{
		auto remainder = std::string(it + 1, it + rem.size() + 1);
		return remainder == rem;
	};

	for (auto pointer = url_string.begin(); pointer != url_string.end(); pointer++)
	{
		u32 code_point = *pointer;
		switch (state)
		{
			case State::SchemeStart:
			{
				if (is_ascii_alpha(code_point))
				{
					buffer += tolower(code_point);
					state = State::Scheme;
				}
				break;
			}

			case State::Scheme:
			{
				if (is_ascii_alphanumeric(code_point) ||
					code_point == '+' ||
					code_point == '-' ||
					code_point == '.'
				)
				{
					buffer += tolower(code_point);
				}

				else if (code_point == ':')
				{
					url.set_scheme(buffer);
					buffer = "";
					if (url.scheme() == "file")
					{
						if (!remaining_starts_with(pointer, "//"))
							std::cerr << "File scheme does not start with //";

						state = State::File;
					}

					else if (remaining_starts_with(pointer, "/"))
					{
						state = State::PathOrAuthority;
						pointer++;
					}
				}
				break;
			}

			case State::PathOrAuthority:
			{
				if (code_point == '/')
					state = State::Authority;
				
				else
				{
					state = State::Path;
					pointer--;
				}
				break;
			}

			case State::Authority:
			{
				if (code_point == '@')
				{
					std::cerr << "@ seen in authority state\n";
					at_sign_seen = true;
					buffer = "";
				}

				else if (code_point == '\0' || code_point == '/' || code_point == '?' || code_point == '#')
				{
					if (at_sign_seen && (buffer == ""))
					{
						std::cerr << "Validation error\n";
						return url;
					}

					pointer -= buffer.size() + 1;
					buffer == "";
					state = State::Host;
				}

				else
				{
					buffer += code_point;
				}
				break;
			}

			default:
				break;
		}
	}

	return url;
}
