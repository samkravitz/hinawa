#include "resource.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "http.h"

void load(const Url &url, std::function<void(const std::vector<u8> &)> callback)
{
	std::cout << "Loading url: " << url.path_str() << "\n";
	std::cout << url.to_string() << "\n";

	if (url.scheme() == "file")
	{
		std::ifstream file(url.path_str());
		std::stringstream buffer;
		buffer << file.rdbuf();
		const auto &str = buffer.str();
		auto data = std::vector<u8>(str.begin(), str.end());
		callback(std::move(data));
	}

	else if (url.scheme() == "http" || url.scheme() == "https")
	{
		auto req = Http{ url };
		auto res = req.send();
		const auto &str = res.body();
		auto data = std::vector<u8>(str.begin(), str.end());
		callback(std::move(data));
	}

	else
		std::cout << "Unknown scheme: " << url.scheme() << "\n";
}
