#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace js
{
class AstNode
{
public:
	virtual const char *name() const = 0;

	virtual void print(std::string const &prefix, bool is_left);
	virtual void print(std::string const &title = "");
};

class Program : public AstNode
{
public:
	void add_stmt(std::shared_ptr<AstNode> stmt);

	const char *name() const { return "Program"; }
	void print(std::string const &prefix, bool is_left);

private:
	std::vector<std::shared_ptr<AstNode>> m_stmts;
};
}
