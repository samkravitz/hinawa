#include "http.h"

#include <cassert>
#include <iostream>
#include <sstream>

#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CRLF "\r\n"

Http Http::send() const
{
	const int port = uri.port();
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		perror("error opening socket");

	struct hostent *hp = gethostbyname(uri.host().c_str());
	if (!hp)
		perror("error no such host");

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	memcpy(&addr.sin_addr.s_addr, hp->h_addr, hp->h_length);

	if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		perror("error connecting socket to host");

	SSL_library_init();
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();
	const SSL_METHOD *meth = TLSv1_2_client_method();
	SSL_CTX *ctx = SSL_CTX_new(meth);
	SSL *ssl = SSL_new(ctx);
	if (!ssl)
	{
		perror("Error creating SSL");
		return {};
	}

	SSL_set_fd(ssl, sockfd);
	int err = SSL_connect(ssl);
	if (err <= 0)
	{
		perror("Error creating SSL connection.  err=%x\n");
		return {};
	}

	std::cout << "SSL connection using " << SSL_get_cipher(ssl) << "\n";

	auto req = to_string();
	std::cout << req << "\n";
	uint bytes_sent = SSL_write(ssl, req.c_str(), req.size());
	if (bytes_sent != req.size())
		perror("error writing message to socket");

	char *buf = (char *) malloc(32768);
	bzero(buf, 32768);

	uint bytes_read;

	// first, try to read headers from the network
	if ((bytes_read = SSL_read(ssl, buf, 32768)) < 0)
	{
		std::cerr << "Error reading http headers from network\n";
		free(buf);
		return {};
	}

	// headers were read, so try to parse them to find the content length of body
	Http response;
	response.parse_headers(buf);
	unsigned content_length = 0xffffffff;

	// now, make the second read to the socket to get the body
	if (auto s = response.header("Content-Length"); !s.empty())
	{
		content_length = std::stoi(s);
		std::cout << "Content-Length: " << content_length << "\n";
	}

	std::string body;
	unsigned total_bytes = 0;
	do
	{
		bytes_read = SSL_read(ssl, buf, 32768);
		total_bytes += bytes_read;
		body += std::string(buf, bytes_read);
	} while (total_bytes < content_length && bytes_read > 0);

	response.set_body(body);

	free(buf);
	return response;
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
