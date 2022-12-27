#include "http.h"

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
	std::string res;
	do
	{
		bytes_read = SSL_read(ssl, buf, 1024);
		buf[bytes_read] = 0;
		res += std::string(buf);
	} while (bytes_read > 0);

	// find last instance of /r/f in the response, and sets the body to that position
	// TODO - actually parse the http response
	auto pos = res.rfind(CRLF);
	auto body = res.substr(pos + 1);
	Http response;
	response.set_body(body);

	free(buf);
	return response;
}

std::string Http::to_string() const
{
	std::stringstream ss;
	ss << "GET / HTTP/1.1" << CRLF;
	for (const auto &header : headers)
		ss << header.first << ": " << header.second << CRLF;

	ss << CRLF;

	return ss.str();
}
