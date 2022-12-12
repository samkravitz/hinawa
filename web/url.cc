#include "url.h"

Url::Url(const std::string &url_string) :
    m_url_string(url_string)
{ }

std::string Url::to_string() const
{
	return m_url_string;
}
