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

	print("", false);
}

void Program::add_stmt(std::shared_ptr<AstNode> stmt) { m_stmts.push_back(stmt); }

void Program::print(std::string const &prefix, bool is_left)
{
	std::cout << prefix;
	std::cout << (is_left ? "├──" : "└──");
	std::cout << name() << "\n";

	for (auto stmt : m_stmts)
		stmt->print(prefix + (is_left ? "│   " : "    "), true);
}
}
