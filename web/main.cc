#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * makes an http request to a simple html page I'm hosting on my server,
 * https://www.kravitz.dev/html_basic.html
 */
int main()
{
	const int port = 80;    // default port for http
	const char *request = "GET /html_basic.html HTTP/1.1\r\n"
	                      "Host: www.kravitz.dev\r\n"
	                      "Accept-Language: en-us\r\n"
	                      "\r\n";

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		perror("error opening socket");

	struct hostent *hp = gethostbyname("www.kravitz.dev");
	if (!hp)
		perror("error no such host");

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	memcpy(&addr.sin_addr.s_addr, hp->h_addr, hp->h_length);

	if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		perror("error connecting socket to host");

	size_t req_len = strlen(request);
	int bytes_sent = write(sockfd, request, req_len);
	if (bytes_sent != req_len)
		perror("error writing message to socket");

	char *res = malloc(4096);
	bzero(res, 4096);
	int bytes_recieved = read(sockfd, res, 4096);
	if (bytes_recieved < 0)
		perror("error reading response from socket");

	res[bytes_recieved] = '\0';

	close(sockfd);
	printf("%s", res);
	free(res);
	return 0;
}
