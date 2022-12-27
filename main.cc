#include <filesystem>
#include <string>

namespace fs = std::filesystem;

#include "browser/browser.h"

static const char *homepage_url = "../data/homepage.html";

int main(int argc, char **argv)
{
	std::string html_file = homepage_url;
	if (argc > 1)
	{
		if (fs::exists(argv[1]))
			html_file = argv[1];
	}
	
	auto path = fs::canonical(html_file);
	auto hinawa = browser::Browser("file://" + std::string(path.c_str()));
	return 0;
}
