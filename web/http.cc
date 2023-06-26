#include "http.h"

#include <cassert>
#include <iostream>
#include <sstream>

#include <curl/curl.h>

#define CRLF "\r\n"

Http Http::send() const
{
	CURL *curl = curl_easy_init();
	CURLcode res;

	std::string data;
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, uri.serialize().c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &data);
		curl_easy_setopt(
		    curl, CURLOPT_WRITEFUNCTION, +[](char *ptr, size_t size, size_t nmemb, std::string *stream) -> size_t {
			    stream->append(ptr, size * nmemb);
			    return size * nmemb;
		    });

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";

		curl_easy_cleanup(curl);
	}

	Http http_response{};
	http_response.set_body(data);
	return http_response;
}

void Http::parse_headers(const std::string &input)
{
	assert(!input.empty());

	std::istringstream iss(input);
	std::string header;

	// read the first line, which should be something like
	// GET / HTTP/1.1
	assert(std::getline(iss, header));

	while (std::getline(iss, header) && header != "\r")
	{
		auto index = header.find(':');
		if (index == std::string::npos)
		{
			std::cerr << "Invalid HTTP header: " << header << "\n";
			break;
		}

		auto value = header.substr(index + 2);
		value.pop_back();
		headers[header.substr(0, index)] = value;
	}

	for (const auto &h : headers)
		std::cout << h.first << ": " << h.second << "\n";
}

std::string Http::header(const std::string &key)
{
	if (headers.find(key) == headers.end())
		return {};

	return headers[key];
}

std::string Http::to_string() const
{
	std::stringstream ss;
	ss << "GET ";
	ss << uri.serialize_path() << " ";
	ss << "HTTP/1.1";
	ss << CRLF;

	for (const auto &header : headers)
		ss << header.first << ": " << header.second << CRLF;

	ss << CRLF;

	return ss.str();
}
