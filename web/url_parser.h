#pragma once

#include <optional>

#include "url.h"

#define STATES                           \
	STATE(SchemeStart)                   \
	STATE(Scheme)                        \
	STATE(NoScheme)                      \
	STATE(SpecialRelativeOrAuthority)    \
	STATE(PathOrAuthority)               \
	STATE(Relative)                      \
	STATE(RelativeSlash)                 \
	STATE(SpecialAuthoritySlashes)       \
	STATE(SpecialAuthorityIgnoreSlashes) \
	STATE(Authority)                     \
	STATE(Host)                          \
	STATE(Hostname)                      \
	STATE(Port)                          \
	STATE(File)                          \
	STATE(FileSlash)                     \
	STATE(FileHost)                      \
	STATE(PathStart)                     \
	STATE(Path)                          \
	STATE(OpaquePath)                    \
	STATE(Query)                         \
	STATE(Fragment)

class UrlParser
{
public:
	enum class State
	{
#define STATE(state) state,
		STATES
#undef STATE
	};

	Url parse(const std::string &input, Url* base = nullptr, std::optional<State> state_override = {});

private:
	State state = State::SchemeStart;
	std::string buffer = "";
	bool at_sign_seen = false;
	bool inside_brackets = false;
	bool password_token_seen = false;
	State state_override_state = State::SchemeStart;

	std::string parse_host(const std::string &input, bool is_not_special = false);
	std::string parse_ipv6(const std::string &);
	std::string parse_opaque_host(const std::string &);
	std::string utf8_decode_without_bom(const std::string &);
	std::string percent_decode(const std::string &);
	void validation_error(const std::string &msg);
};

#undef STATES
