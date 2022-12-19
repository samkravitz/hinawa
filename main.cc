#include <string>

#include "browser/browser.h"

static const char *homepage_url = "file:///home/sam/projects/hinawa/data/homepage.html";

int main(int argc, char **argv)
{
	std::string html_file = homepage_url;
	if (argc > 1)
		html_file = argv[1];

	auto hinawa = browser::Browser(html_file);
	return 0;
}
