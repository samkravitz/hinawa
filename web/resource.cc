#include "resource.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "http.h"

void load(const Url &url, std::function<void(const std::vector<u8> &)> callback)
{
	if (url.scheme() == "file")
	{
		std::ifstream file(url.path_str());
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::vector<u8> data = { buffer.str().begin(), buffer.str().end() };
		callback(data);
	}

	else if (url.scheme() == "http" || url.scheme() == "https")
	{
		auto req = Http{ url };
		auto res = req.send();
		std::vector<u8> data = { res.body().begin(), res.body().end() };
		callback(data);
	}

	else
		std::cout << "Unknown scheme: " << url.scheme() << "\n";
}
