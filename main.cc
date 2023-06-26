#include <QApplication>
#include <curl/curl.h>
#include <filesystem>
#include <string>

#include "browser/browser_window.h"
#include "util/hinawa.h"

static const char *homepage_url = "index.html";

int main(int argc, char **argv)
{
	std::string html_file = DATA_DIR / "homepage" / homepage_url;
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
	QApplication app(argc, argv);
	auto hinawa = browser::BrowserWindow(url);
	auto ret = app.exec();
	curl_global_cleanup();
	return ret;
}
