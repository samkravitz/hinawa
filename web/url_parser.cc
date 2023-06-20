#include "url_parser.h"

#include <cassert>
#include <cctype>
#include <iostream>

#include "util/hinawa.h"

static inline bool is_ascii_digit(u32 c)
{
	return c >= '0' && c <= '9';
}
static inline bool is_ascii_upper_hex_digit(u32 c)
{
	return c >= 'A' && c <= 'F';
}
static inline bool is_ascii_lower_hex_digit(u32 c)
{
	return c >= 'a' && c <= 'f';
}
static inline bool is_ascii_upper_alpha(u32 c)
{
	return c >= 'A' && c <= 'Z';
}
static inline bool is_ascii_lower_alpha(u32 c)
{
	return c >= 'a' && c <= 'z';
}
static inline bool is_ascii_alpha(u32 c)
{
	return is_ascii_upper_alpha(c) || is_ascii_lower_alpha(c);
}
static inline bool is_ascii_alphanumeric(u32 c)
{
	return is_ascii_alpha(c) || is_ascii_digit(c);
}

// https://url.spec.whatwg.org/#url-code-points
static inline bool is_url_codepoint(u32 c)
{
	return is_ascii_alphanumeric(c) || c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' ||
	       c == '*' || c == '+' || c == ',' || c == '-' || c == '.' || c == '/' || c == ':' || c == ';' || c == '=' ||
	       c == '?' || c == '@' || c == '_' || c == '~' || (c >= 0xa0 && c <= 0x10fffd);
}

// https://url.spec.whatwg.org/#concept-domain-to-ascii
// TODO
static std::string domain_to_ascii(const std::string &input, bool be_strict)
{
	return input;
}

// implements the basic URL parser spec
// https://url.spec.whatwg.org/#concept-basic-url-parser
Url UrlParser::parse(const std::string &input, Url* base, std::optional<State> state_override)
{
	Url url;
	buffer = "";
	bool state_override_given = state_override.has_value();
	if (state_override_given)
		state = state_override.value();

	// current code point
	u32 c;

	auto remaining_starts_with = [](auto const &it, std::string const &rem) -> bool {
		auto remainder = std::string(it + 1, it + rem.size() + 1);
		return remainder == rem;
	};

start_over:
	for (auto pointer = input.begin();; pointer++)
	{
		c = '\0';
		if (pointer != input.end())
			c = *pointer;

		switch (state)
		{
			case State::SchemeStart:
			{
				// 1. If c is an ASCII alpha, append c, lowercased, to buffer, and set state to scheme state
				if (is_ascii_alpha(c))
				{
					buffer += tolower(c);
					state = State::Scheme;
				}

				// 2. Otherwise, if state override is not given, set state to no scheme state and decrease pointer by 1
				else if (!state_override)
				{
					state = State::NoScheme;
					pointer--;
				}

				// 3. Otherwise, validation error, return failure
				else
				{
					validation_error("Invalid in scheme start state");
					return url;
				}
				break;
			}

			case State::Scheme:
			{
				// 1. If c is an ASCII alphanumeric, U+002B (+), U+002D (-), or U+002E (.), append c, lowercased, to buffer
				if (is_ascii_alphanumeric(c) || c == '+' || c == '-' || c == '.')
					buffer += tolower(c);

				// 2. Otherwise, if c is U+003A (:), then:
				else if (c == ':')
				{
					// 1. If state override is given, then:
					if (state_override_given)
					{
						// 1. If url’s scheme is a special scheme and buffer is not a special scheme, then return
						// TODO

						// 2. If url’s scheme is not a special scheme and buffer is a special scheme, then return
						// TODO

						// 3. If url includes credentials or has a non-null port, and buffer is "file", then return
						// TODO

						// 4. If url’s scheme is "file" and its host is an empty host, then return
						// TODO
					}

					// 2. Set url’s scheme to buffer
					url.set_scheme(buffer);

					// 3. If state override is given, then:
					if (state_override_given)
					{
						// 1. If url’s port is url’s scheme’s default port, then set url’s port to null
						if (url.port() == Url::default_port(url.scheme()))
							url.set_port(PORT_NULL);

						// 2. Return
						return url;
					}

					// 4. Set buffer to the empty string
					buffer = "";

					// 5. If url's scheme is "file", then:
					if (url.scheme() == "file")
					{
						// 1. If remaining does not start with "//", validation error
						if (!remaining_starts_with(pointer, "//"))
							validation_error("File scheme does not start with //");

						// 2. Set state to file state
						state = State::File;
					}

					// 6. Otherwise, if url is special, base is non-null, and base’s scheme is url’s scheme:
					else if (url.is_special() && base && (base->scheme() == url.scheme()))
					{
						// 1. Assert: base is is special (and therefore does not have an opaque path)
						assert(base->is_special());

						// 2. Set state to special relative or authority state
						state = State::SpecialRelativeOrAuthority;
					}

					// 7. Otherwise, if url is special, set state to special authority slashes state
					else if (url.is_special())
						state = State::SpecialAuthoritySlashes;

					// 8. Otherwise, if remaining starts with an U+002F (/), set state to path or authority state and increase pointer by 1
					else if (remaining_starts_with(pointer, "/"))
					{
						state = State::PathOrAuthority;
						pointer++;
					}

					// 9. Otherwise, set url’s path to the empty string and set state to opaque path state
					else
					{
						url.clear_path();
						state = State::OpaquePath;
					}
				}

				// 3. Otherwise, if state override is not given, set buffer to the empty string, state to no scheme state, and start over (from the first code point in input)
				else if (!state_override_given)
				{
					buffer = "";
					state = State::NoScheme;
					goto start_over;
				}

				// 4. Otherwise, validation error, return failure
				else
				{
					validation_error("Error in scheme state");
					return url;
				}
				break;
			}

			case State::NoScheme:
			{
				// 1. If base is null, or base has an opaque path and c is not U+0023 (#), validation error, return failure
				if (!base || (base->has_opaque_path() && c != '#'))
				{
					validation_error("base is null or has an opaque path");
					return url;
				}

				// 2. Otherwise, if base has an opaque path and c is U+0023 (#), set url's scheme to base's scheme,
				// url's path to base's path, url's query to base's query, url's fragment to the empty string, and set state to fragment state
				else if (base->has_opaque_path() && c == '#')
				{
					url.set_scheme(base->scheme());
					url.set_path(base->path());
					url.set_query(base->query());
					url.set_fragment(base->fragment());
					state = State::Fragment;
				}

				// 3. Otherwise, if base's scheme is not "file", set state to relative state and decrease pointer by 1
				else if (base->scheme() != "file")
				{
					state = State::Relative;
					pointer--;
				}

				// 4. Otherwise, set state to file state and decrease pointer by 1
				else
				{
					state = State::File;
					pointer--;
				}
				break;
			}

			case State::SpecialRelativeOrAuthority:
			{
				// 1. If c is U+002F (/) and remaining starts with U+002F (/), then set state to special authority ignore slashes state and increase pointer by 1
				if (c == '/' && remaining_starts_with(pointer, "/"))
				{
					state = State::SpecialAuthorityIgnoreSlashes;
					pointer++;
				}

				// 2. Otherwise, validation error, set state to relative state and decrease pointer by 1
				else
				{
					validation_error("rest doesn't start with /");
					pointer--;
				}

				break;
			}

			case State::PathOrAuthority:
			{
				// 1. If c is U+002F (/), then set state to authority state
				if (c == '/')
					state = State::Authority;

				// 2. Otherwise, set state to path state, and decrease pointer by 1
				else
				{
					state = State::Path;
					pointer--;
				}
				break;
			}

			case State::Relative:
			{
				assert(base);
				// 1. Assert: base’s scheme is not "file"
				assert(base->scheme() != "file");

				// 2. Set url’s scheme to base’s scheme
				url.set_scheme(base->scheme());

				// 3. If c is U+002F (/), then set state to relative slash state
				if (c == '/')
					state = State::RelativeSlash;

				// 4. Otherwise, if url is special and c is U+005C (\), validation error, set state to relative slash state
				else if (url.is_special() && c == '\\')
				{
					validation_error("Invalid in relative state");
					state = State::RelativeSlash;
				}

				// 5. Otherwise
				else
				{
					// 1. Set url’s username to base’s username, url’s password to base’s password, url’s host to base’s host, url’s port to base’s port, url’s path to a clone of base’s path, and url’s query to base’s query
					// TODO
					url.set_host(base->host());
					url.set_port(base->port());
					url.set_path(base->path());
					url.set_query(base->query());

					// 2. If c is U+003F (?), then set url’s query to the empty string, and state to query state
					if (c == '?')
					{
						url.set_query("");
						state = State::Query;
					}

					// 3. Otherwise, if c is U+0023 (#), set url’s fragment to the empty string and state to fragment state
					else if (c == '#')
					{
						url.set_fragment("");
						state = State::Fragment;
					}

					// 4. Otherwise, if c is not the EOF code point
					else if (c != '\0')
					{
						// 1. Set url’s query to null
						url.set_query("");

						// 2. Shorten url’s path
						url.shorten_path();

						// 3. Set state to path state and decrease pointer by 1
						state = State::Path;
						pointer--;
					}
				}

				break;
			}

			case State::RelativeSlash:
			{
				// 1. If url is special and c is U+002F (/) or U+005C (\), then:
				if (url.is_special() && (c == '/' || c == '\\'))
				{
					// 1. If c is U+005C (\), validation error
					if (c == '\\')
						validation_error("c is \\");

					// 2. Set state to special authority ignore slashes state
					state = State::SpecialAuthorityIgnoreSlashes;
				}

				// 2. Otherwise, if c is U+002F (/), then set state to authority state
				else if (c == '/')
					state = State::Authority;

				// 3. Otherwise, set url’s username to base’s username, url’s password to base’s password, url’s host to base’s host,
				// url’s port to base’s port, state to path state, and then, decrease pointer by 1.
				// TODO
				else
				{
					url.set_host(base->host());
					url.set_port(base->port());
					state = State::Path;
					pointer--;
				}
				break;
			}

			case State::SpecialAuthoritySlashes:
			{
				// 1. If c is U+002F (/) and remaining starts with U+002F (/), then set state to special authority ignore slashes state and increase pointer by 1
				if (c == '/' && (remaining_starts_with(pointer, "/")))
				{
					state = State::SpecialAuthorityIgnoreSlashes;
					pointer++;
				}

				// 2. Otherwise, validation error, set state to special authority ignore slashes state and decrease pointer by 1
				else
				{
					validation_error("invalid special authority slashes");
					pointer--;
				}
				break;
			}

			case State::SpecialAuthorityIgnoreSlashes:
			{
				// 1. If c is neither U+002F (/) nor U+005C (\), then set state to authority state and decrease pointer by 1
				if (c != '/' && c != '\\')
				{
					state = State::Authority;
					pointer--;
				}

				// 2. Otherwise, validation error
				else
					validation_error("special authority ignore slahes");
				break;
			}

			case State::Authority:
			{
				// 1. If c is U+0040 (@), then:
				if (c == '@')
				{
					// 1. Validation error
					validation_error("@ in authority state");

					// 2. If atSignSeen is true, then prepend "%40" to buffer
					if (at_sign_seen)
						buffer += "%40";

					// 3. Set atSignSeen to true
					at_sign_seen = true;

					// 4. For each codePoint in buffer
					for (const auto codepoint : buffer)
					{
						// 1. If codePoint is U+003A (:) and passwordTokenSeen is false, then set passwordTokenSeen to true and continue
						if (codepoint == ':' && !password_token_seen)
						{
							password_token_seen = true;
							continue;
						}

						// 2. Let encodedCodePoints be the result of running UTF-8 percent-encode codePoint using the userinfo percent-encode set
						// TODO

						// 3. If passwordTokenSeen is true, then append encodedCodePoints to url's password
						// TODO

						// 4. Otherwise, append encodedCodePoints to url's username
						// TODO
					}

					// 5. Set buffer to the empty string
					buffer = "";
				}

				// 2. Otherwise, if one of the following is true:
				//   - c is the EOF code point, U+002F (/), U+003F (?), or U+0023 (#)
				//   - url is special and c is U+005C (\)
				else if ((c == '\0' || c == '/' || c == '?' || c == '#') || (url.is_special() && c == '\\'))
				{
					// 1. If atSignSeen is true and buffer is the empty string, validation error, return failure
					if (at_sign_seen && (buffer == ""))
					{
						validation_error("atSignSeen is true and buffer is empty");
						return url;
					}

					// 2. Decrease pointer by the number of code points in buffer plus one, set buffer to the empty string, and set state to host state
					pointer -= buffer.size() + 1;
					buffer = "";
					state = State::Host;
					continue;
				}

				// 3. Otherwise, append c to buffer
				else
					buffer += c;
				break;
			}

			case State::Host:
			case State::Hostname:
			{
				// 1. If state override is given and url’s scheme is "file", then decrease pointer by 1 and set state to file host state
				if (state_override_given && url.scheme() == "file")
				{
					pointer--;
					state = State::FileHost;
				}

				// 2. Otherwise, if c is U+003A (:) and insideBrackets is false, then
				else if (c == ':' && !inside_brackets)
				{
					// 1. If buffer is the empty string, validation error, return failure
					if (buffer == "")
					{
						validation_error("buffer is empty string");
						return url;
					}

					// 2. If state override is given and state override is hostname state, then return
					if (state_override_given && state_override.value() == State::Hostname)
					{
						return url;
					}

					// 3. Let host be the result of host parsing buffer with url is not special
					// TODO

					// 4. If host is failure, then return failure.

					// 5. Set url’s host to host, buffer to the empty string, and state to port state.
				}

				// 3. Otherwise, if one of the following is true:
				//   - c is the EOF code point, U+002F (/), U+003F (?), or U+0023 (#)
				//   - url is special and c is U+005C (\)
				else if ((c == '\0' || c == '/' || c == '?' || c == '#') || (url.is_special() && c == '\\'))
				{
					// then decrease pointer by 1, and then:
					pointer--;

					//  1. If url is special and buffer is the empty string, validation error, return failure
					if (url.is_special() && buffer == "")
					{
						validation_error("buffer is empty string");
						return url;
					}

					// 2. Otherwise, if state override is given, buffer is the empty string, and either url includes credentials or url’s port is non-null, return
					// TODO

					// 3. Let host be the result of host parsing buffer with url is not special
					auto host = parse_host(buffer);

					// 4. If host is failure, then return failure

					// 5. Set url’s host to host, buffer to the empty string, and state to path start state
					url.set_host(host);
					buffer = "";
					state = State::PathStart;

					// 6. If state override is given, then return
					if (state_override_given)
						return url;
				}

				// Otherwise:
				else
				{
					// If c is U+005B ([), then set insideBrackets to true
					if (c == '[')
						inside_brackets = true;

					// If c is U+005D (]), then set insideBrackets to false
					if (c == ']')
						inside_brackets = false;

					// Append c to buffer.
					buffer += c;
				}

				break;
			}

			case State::Port:
			{
				// 1. If c is an ASCII digit, append c to buffer
				if (is_ascii_digit(c))
					buffer += c;

				// 2. Otherwise, if one of the following is true:
				//   - c is the EOF code point, U+002F (/), U+003F (?), or U+0023 (#)
				//   - url is special and c is U+005C (\)
				//   - state override is given
				else if ((c == '\0' || c == '/' || c == '?' || c == '#') || (url.is_special() && c == '\\') ||
				         state_override_given)
				{
					// 1. If buffer is not the empty string, then:
					if (buffer != "")
					{
						// 1. Let port be the mathematical integer value that is represented by buffer in radix-10 using ASCII digits for digits with values 0 through 9
						int port = std::stoi(buffer);

						// 2. If port is greater than 2^16 − 1, validation error, return failure
						if (port >= 2 << 15)
						{
							validation_error("port is greater than 2^16 - 1");
							return url;
						}

						// 3. Set url’s port to null, if port is url’s scheme’s default port; otherwise to port
						if (port == Url::default_port(url.scheme()))
							url.set_port(PORT_NULL);
						else
							url.set_port(port);

						// 4. Set buffer to the empty string
						buffer = "";
					}

					// 2. If state override is given, then return
					if (state_override_given)
						return url;

					// 3. Set state to path start state and decrease pointer by 1
					state = State::PathStart;
					pointer--;
				}

				// 4. Otherwise, validation error, return failure
				else
				{
					validation_error("validation error in port state");
					return url;
				}
			}

			case State::File:
			{
				// 1. Set url's scheme to "file"
				url.set_scheme("file");

				// 2. Set url's host to the empty string
				url.set_host("");

				// 3. If c is U+002F (/) or U+005C (\), then:
				if (c == '/' || c == '\\')
				{
					// 1. If c is U+005C (\), validation error
					if (c == '\\')
						validation_error("c is \\");

					// 2. Set state to file slash state
					state = State::FileSlash;
				}

				// 4. Otherwise, if base is non-null and base's scheme is "file":
				else if (base && base->scheme() == "file")
				{
					// 1. Set url's host to base's host, url's path to a clone of base's path, and url's query to base's query
					url.set_host(base->host());
					url.set_path(base->path());
					url.set_query(base->query());

					// 2. If c is U+003F (?), then set url's query to the empty string and state to query state
					if (c == '?')
					{
						url.set_query("");
						state = State::Query;
					}

					// 3. Otherwise, if c is U+0023 (#), set url's fragment to the empty string and state to fragment state
					else if (c == '#')
					{
						url.set_fragment("");
						state = State::Fragment;
					}

					// 4. Otherwise, if c is not the EOF code point:
					else if (c != '\0')
					{
						// 1. Set url's query to null
						url.set_query("");

						// 2. If the code point substring from pointer to the end of input does not start with a Windows drive letter, then shorten url's path
						// TODO
						if (true)
							url.shorten_path();

						// 3. Otherwise:
						else
						{
							// 1. Validation error
							validation_error("substring doesn't start with Windows drive letter");

							// 2. Set url's path to an empty list
							url.clear_path();
						}

						// 4. Set state to path state and decrease pointer by 1
						state = State::Path;
						pointer--;
					}
				}

				// 5. Otherwise, set state to path state, and decrease pointer by 1
				else
				{
					state = State::Path;
					pointer--;
				}

				break;
			}

			case State::FileSlash:
			{
				// 1. If c is U+002F (/) or U+005C (\), then:
				if (c == '/' || c == '\\')
				{
					// 1. If c is U+005C (\), validation error
					if (c == '\\')
						validation_error("c is \\");

					// 2. Set state to file host state
					state = State::FileHost;
				}

				// 2. Otherwise
				else
				{
					// 1. if base is non-null and base's scheme is "file", then:
					if (base && base->scheme() == "file")
					{
						// 1. Set url's host to base's host
						url.set_host(base->host());

						// 2. If the code point substring from pointer to the end of input does not start with a Windows drive letter
						// and base's path[0] is a normalized Windows drive letter, then append base's path[0] to url's path
						// TODO
					}

					// 2. Set state to path state, and decrease pointer by 1
					state = State::Path;
					pointer--;
				}

				break;
			}

			case State::FileHost:
			{
				// 1. If c is the EOF code point, U+002F (/), U+005C (\), U+003F (?), or U+0023 (#), then decrease pointer by 1 and then:
				if (c == '\0' || c == '/' || c == '\\' || c == '?' || c == '#')
				{
					pointer--;

					// 1. If state override is not given and buffer is a Windows drive letter, validation error, set state to path state
					// TODO
					if (false)
						;

					// 2. This is a (platform-independent) Windows drive letter quirk. buffer is not reset here and instead used in the path state
					// TODO

					// 3. Otherwise, if buffer is the empty string, then:
					else if (buffer == "")
					{
						// 1. Set url’s host to the empty string
						url.set_host("");

						// 2. If state override is given, then return
						if (state_override_given)
							return url;

						// 3. Set state to path start state
						state = State::PathStart;
					}

					// 3. Otherwise, run these steps:
					else
					{
						// 1. Let host be the result of host parsing buffer with url is not special
						auto host = parse_host(buffer);

						// 2. If host is failure, then return failure
						// TODO

						// 3. If host is "localhost", then set host to the empty string
						if (host == "localhost")
							host = "";

						// 4. Set url’s host to host
						url.set_host(host);

						// 5. If state override is given, then return
						if (state_override_given)
							return url;

						// 6. Set buffer to the empty string and state to path start state
						buffer = "";
						state = State::PathStart;
					}
				}

				// 3. Otherwise, append c to buffer.
				else
					buffer += c;

				break;
			}

			case State::PathStart:
			{
				// 1. If url is special, then:
				if (url.is_special())
				{
					// 1. If c is U+005C (\), validation error
					if (c == '\\')
						validation_error("c is \\");

					// 2. Set state to path state
					state = State::Path;

					// 3. If c is neither U+002F (/) nor U+005C (\), then decrease pointer by 1
					if (c != '/' && c != '\\')
						pointer--;
				}

				// 2. Otherwise, if state override is not given and c is U+003F (?), set url’s query to the empty string and state to query state
				else if (!state_override_given && c == '?')
				{
					url.set_query("");
					state = State::Query;
				}

				// 3. Otherwise, if state override is not given and c is U+0023 (#), set url’s fragment to the empty string and state to fragment state
				else if (!state_override_given && c == '#')
				{
					url.set_fragment("");
					state = State::Fragment;
				}

				// 4. Otherwise, if c is not the EOF code point:
				else if (c != '\0')
				{
					// 1. Set state to path state
					state = State::Path;

					// 2. If c is not U+002F (/), then decrease pointer by 1
					if (c != '/')
						pointer--;
				}

				// 5. Otherwise, if state override is given and url’s host is null, append the empty string to url’s path
				else if (state_override_given && url.host() == "")
					url.append_path("");

				break;
			}

			case State::Path:
			{
				// If one of the following is true:
				// * c is the EOF code point or U+002F (/)
				// * url is special and c is U+005C (\)
				// * state override is not given and c is U+003F (?) or U+0023 (#)

				if ((c == '\0' || c == '/') || (c == '?') || (c == '?' || c == '#'))
				{
					// 1. If url is special and c is U+005C (\), validation error
					if (url.is_special() && c == '\\')
						validation_error("url is special and c is \\");

					// 2. If buffer is a double-dot path segment, then:
					if (buffer == "..")
					{
						// 1. Shorten url's path
						url.shorten_path();

						// 2. If neither c is U+002F (/), nor url is special and c is U+005C (\), append the empty string to url's path
						if (c != '/' && !(url.is_special() && c == '\\'))
							url.append_path("");
					}

					// 3. Otherwise, if buffer is a single-dot path segment and if neither c is U+002F (/), nor url is special and c is U+005C (\), append the empty string to url's path.
					else if (buffer == "." && (c != '/' && !(url.is_special() && c == '\\')))
						url.append_path("");

					// 4. Otherwise, if buffer is not a single-dot path segment, then
					else if (buffer != ".")
					{
						// 1. If url's scheme is "file", url's path is empty, and buffer is a Windows drive letter, then replace the second code point in buffer with U+003A (:)
						// TODO

						// 2. Append buffer to url's path
						url.append_path(buffer);
					}

					// 5. Set buffer to the empty string
					buffer = "";

					// 6. If c is U+003F (?), then set url's query to the empty string and state to query state
					if (c == '?')
					{
						url.set_query("");
						state = State::Query;
					}

					// 7. If c is U+0023 (#), then set url's fragment to the empty string and state to fragment state
					if (c == '#')
					{
						url.set_fragment("");
						state = State::Fragment;
					}
				}

				// 2. Otherwise, run these steps
				else
				{
					// 1. If c is not a URL code point and not U+0025 (%), validation error
					if (!is_url_codepoint(c) && c != '%')
						validation_error("codepoint is not url codepoint");

					// 2. If c is U+0025 (%) and remaining does not start with two ASCII hex digits, validation error
					// TODO

					// 3. UTF-8 percent-encode c using the path percent-encode set and append the result to buffer
					// TODO
					buffer += c;
				}

				break;
			}

			case State::OpaquePath:
			{
				break;
			}

			case State::Query:
			{
				break;
			}

			case State::Fragment:
			{
				// 1. If c is not the EOF code point, then:
				if (c != '\0')
				{
					// 1. If c is not a URL code point and not U+0025 (%), validation error
					if (!is_url_codepoint(c) && c != '%')
						validation_error("fragment state invalid");

					// 2. If c is U+0025 (%) and remaining does not start with two ASCII hex digits, validation error.
					// TODO
					if (c == '%')
						validation_error("does not start with 2 ascii digits");

					// 3. UTF-8 percent-encode c using the fragment percent-encode set and append the result to url’s fragment
					// TODO
				}
				break;
			}
		}

		if (pointer == input.end())
			break;
	}

	return url;
}

// https://url.spec.whatwg.org/#concept-host-parser
std::string UrlParser::parse_host(const std::string &input, bool is_not_special)
{
	// 1. If input starts with U+005B ([), then:
	if (input.rfind("[", 0) == 0)
	{
		// 1. If input does not end with U+005D (]), validation error, return failure.
		if (input.back() != ']')
			;

		// 2. Return the result of IPv6 parsing input with its leading U+005B ([) and trailing U+005D (]) removed
		return parse_ipv6(input);
	}

	// 2. If isNotSpecial is true, then return the result of opaque-host parsing input
	if (is_not_special)
		return parse_opaque_host(input);

	// 3. Assert: input is not the empty string
	assert(input != "");

	// 4. Let domain be the result of running UTF-8 decode without BOM on the percent-decoding of input
	auto domain = utf8_decode_without_bom(percent_decode(input));

	// 5. Let asciiDomain be the result of running domain to ASCII with domain and false
	auto ascii_domain = domain_to_ascii(domain, false);

	// 6. If asciiDomain is failure, validation error, return failure
	// TODO

	// 7. If asciiDomain contains a forbidden domain code point, validation error, return failure
	// TODO

	// 8. If asciiDomain ends in a number, then return the result of IPv4 parsing asciiDomain
	// TODO

	// 9. Return asciiDomain
	return ascii_domain;
}

// https://url.spec.whatwg.org/#concept-ipv6-parser
// TODO
std::string UrlParser::parse_ipv6(const std::string &input)
{
	return input;
}

// https://url.spec.whatwg.org/#concept-opaque-host-parser
// TODO
std::string UrlParser::parse_opaque_host(const std::string &input)
{
	return input;
}

// https://encoding.spec.whatwg.org/#utf-8-decode-without-bom
// TODO
std::string UrlParser::utf8_decode_without_bom(const std::string &input)
{
	return input;
}

// https://url.spec.whatwg.org/#string-percent-decode
// TODO
std::string UrlParser::percent_decode(const std::string &input)
{
	return input;
}

void UrlParser::validation_error(const std::string &msg)
{
	std::cerr << "Validation error: " << msg << "\n";
}
