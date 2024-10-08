#include "url.h"

#include <cassert>
#include <iostream>
#include <sstream>

#include "url_parser.h"

Url::Url(const std::string &url_string) :
    Url(UrlParser().parse(url_string))
{ }

Url::Url(const std::string &url_string, Url *base) :
    Url(UrlParser().parse(url_string, base, {}))
{ }

// https://url.spec.whatwg.org/#default-port
int Url::default_port(const std::string &scheme)
{
	if (scheme == "ftp")
		return FTP_DEFAULT_PORT;

	if (scheme == "file")
		return PORT_NULL;

	if (scheme == "http")
		return HTTP_DEFAULT_PORT;

	if (scheme == "https")
		return HTTPS_DEFAULT_PORT;

	if (scheme == "ws")
		return HTTP_DEFAULT_PORT;

	if (scheme == "wss")
		return HTTPS_DEFAULT_PORT;

	return PORT_NULL;
}

// https://url.spec.whatwg.org/#is-special
bool Url::is_special() const
{
	if (scheme() == "ftp")
		return true;

	if (scheme() == "file")
		return true;

	if (scheme() == "http")
		return true;

	if (scheme() == "https")
		return true;

	if (scheme() == "ws")
		return true;

	if (scheme() == "wss")
		return true;

	return false;
}

// https://url.spec.whatwg.org/#shorten-a-urls-path
void Url::shorten_path()
{
	// 1. Assert: url does not have an opaque path
	assert(!has_opaque_path());

	// 2. Let path be url's path
	auto p = path();

	// 3. If url's scheme is "file", path's size is 1, and path[0] is a normalized Windows drive letter, then return
	if (scheme() == "file" && p.size() == 1 && p[0] == "c")
		return;

	// 4. Remove path's last item, if any
	m_path.pop_back();
}

int Url::port() const
{
	if (m_port == PORT_NULL && is_special())
		return default_port(scheme());

	return m_port;
}

// https://url.spec.whatwg.org/#concept-url-serializer
std::string Url::serialize(bool exclude_fragment) const
{
	// 1. Let output be url's scheme and U+003A (:) concatenated.
	std::string output = scheme() + ":";

	// 2. If url's host is non-null:
	if (1)
	{
		// 1. Append "//" to output.
		output += "//";

		// 2. If url includes credentials, then:
		if (false)
		{
			// 1. Append url's username to output.
			// 2. If url's password is not the empty string, then append U+003A (:), followed by url's password, to output.
			// 3. Append U+0040 (@) to output.
		}

		// 3. Append url's host, serialized, to output.
		output += host();

		// 4. If url's port is non-null, append U+003A (:) followed by url's port, serialized, to output.
		if (m_port != PORT_NULL)
		{
			output += ":" + std::to_string(port());
		}
	}

	// 3. If url's host is null, url does not have an opaque path,
	// url's path's size is greater than 1, and url's path[0] is the empty string, then append U+002F (/) followed by U+002E (.) to output.
	if (false)
	{
		output += "/.";
	}

	// 4. Append the result of URL path serializing url to output.
	output += serialize_path();

	// 5. If url's query is non-null, append U+003F (?), followed by url's query, to output.

	// 6. If exclude fragment is false and url's fragment is non-null, then append U+0023 (#), followed by url's fragment, to output.

	// 7. Return output.
	return output;
}

// https://url.spec.whatwg.org/#url-path-serializer
std::string Url::serialize_path() const
{
	// 1. If url has an opaque path, then return url's path
	if (has_opaque_path())
		return path()[0];

	// 2. Let output be the empty string
	std::string output = "";

	// 3. For each segment of url's path: append '/' followed by segment to output.
	for (const auto &segment : path())
		output += "/" + segment;

	// 4. return output
	return output;
}

std::string Url::to_string() const
{
	std::stringstream ss;
	ss << "scheme: " << scheme() << "\n";
	ss << "authority: " << authority() << "\n";
	ss << "host: " << host() << "\n";
	ss << "port: " << port() << "\n";
	ss << "drive: " << drive() << "\n";
	ss << "path_root: " << path_root() << "\n";
	ss << "dir: " << dir() << "\n";
	ss << "file: " << file() << "\n";
	ss << "query: " << query() << "\n";
	ss << "fragment: " << fragment() << "\n";

	ss << "path: /";
	auto fullpath = path();
	for (uint i = 0; i < fullpath.size(); i++)
	{
		ss << fullpath[i];
		if (i != fullpath.size() - 1)
			ss << "/";
	}
	ss << "\n";

	return ss.str();
}
