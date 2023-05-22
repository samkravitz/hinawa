#include <curl/curl.h>
#include <filesystem>
#include <string>

#include "browser/browser.h"
#include "util/hinawa.h"

static const char *homepage_url = "homepage.html";

int main(int argc, char **argv)
{
	std::string html_file = DATA_DIR / homepage_url;
	if (argc > 1)
	{
		html_file = argv[1];
	}

	Url url;
	if (fs::exists(html_file))
		url = Url("file://" + fs::canonical(html_file).string());

	else
		url = Url(html_file);

	curl_global_init(CURL_GLOBAL_ALL);
	auto hinawa = browser::Browser(url);
	curl_global_cleanup();
	return 0;
}
