#pragma once

#include "url.h"

#define STATES \
	STATE(SchemeStart) \
	STATE(Scheme) \
	STATE(NoScheme) \
	STATE(SpecialRelativeOrAuthority) \
	STATE(PathOrAuthority) \
	STATE(Relative) \
	STATE(RelativeSlash) \
	STATE(SpecialAuthoritySlashes) \
	STATE(SpecialAuthorityIgnoreSlashes) \
	STATE(Authority) \
	STATE(Host) \
	STATE(Hostname) \
	STATE(Port) \
	STATE(File) \
	STATE(FileSlash) \
	STATE(FileHost) \
	STATE(PathStart) \
	STATE(Path) \
	STATE(CannotBeABaseUrlPath) \
	STATE(Query) \
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

	Url parse(std::string url_str);

private:
	State state = State::SchemeStart;
	std::string buffer = "";
	bool at_sign_seen = false;
	bool inside_brackets = false;
	bool password_token_seen =false;
};

#undef STATES
