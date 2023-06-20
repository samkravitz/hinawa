#include <iostream>

#include "http.h"

int main()
{
	auto req = Http{"https://serenityos.org"};
	auto res = req.send();

	std::cout << res.body() << "\n";
}
