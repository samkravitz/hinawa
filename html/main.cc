#include "tokenizer.h"

int main()
{
	std::string input = "<!DOCTYPE html>\n"
	                    "<html>\n"
	                    "<body>\n"
	                    "<h1>My First Heading</h1>\n"
	                    "<p>My first paragraph.</p>\n"
	                    "</body>\n"
	                    "</html>\n";

	auto tokenizer = html::Tokenizer(input);
	tokenizer.run();
}
