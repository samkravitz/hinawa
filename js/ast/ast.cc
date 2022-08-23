#include "ast.h"

#include "stmt.h"

namespace js
{

void AstNode::print(std::string const &prefix, bool is_left)
{
	std::cout << prefix;
	std::cout << (is_left ? "├──" : "└──");
	std::cout << name() << "\n";
}

void AstNode::print(std::string const &title)
{
	if (!title.empty())
		std::cout << title << "\n";

	print("");
}

}
