#include "url.h"

#include "url_parser.h"

Url::Url(const std::string &url_string) :
	Url(UrlParser().parse(url_string))
{ }

std::string Url::to_string() const
{
	return "";
}
