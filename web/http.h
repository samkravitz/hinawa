#pragma once

#include <string>
#include <unordered_map>

#include "url.h"

enum Method
{
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
};

class Http
{
public:
	Http() = default;
	Http(const std::string &uri_string);
	Http(const Url &url) :
	    uri(url)
	{ }

	Http send() const;

	void set_body(const std::string &body) { m_body = body; }
	std::string body() const { return m_body; }

	std::string to_string() const;

private:
	std::unordered_map<std::string, std::string> headers;
	std::string m_body;
	Method method{ GET };
	Url uri;
};
