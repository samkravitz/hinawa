#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace util
{
template<class T>
class TreeNode
{
public:
	TreeNode() = default;
	~TreeNode() = default;

	virtual std::string to_string() const { return "TREE BASE"; };

	void add_child(std::shared_ptr<T> node)
	{
		node->m_parent = static_cast<T*>(this);
		children.push_back(node);
	}

	TreeNode<T> *parent() const { return m_parent; }

	void preorder(std::function<void(std::shared_ptr<T>)> f)
	{
		for (auto child : children)
		{
			child->preorder(f);
			f(child);
		}
	}

	void postorder(std::function<void(std::shared_ptr<T>)> f)
	{
		for (auto child : children)
		{
			f(child);
			child->postorder(f);
		}
	}

	void for_each_child(std::function<void(std::shared_ptr<T>)> f)
	{
		for (auto child : children)
			f(child);
	}

	std::shared_ptr<T> last_child()
	{
		if (!has_children())
			return nullptr;

		return children.back();
	}

	bool has_children() const { return children.size() != 0; }

	std::size_t size() const
	{
		std::size_t count = 1;
		for (auto child : children)
			count += child->size();

		return count;
	}

	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──"); 
		std::cout << to_string() << "\n";

		int i = 0;
		for (auto child : children)
		{
			if (i++ == 0)
				child->print(prefix + (is_left ? "│   " : "    "), true);
			else
        		child->print(prefix + (is_left ? "│   " : "    "), false);
		}
	}

	void print(std::string const &title = "")
	{
		if (!title.empty())
			std::cout << title << "\n";

		print("", false);
	}

protected:
	T *m_parent = nullptr;
	std::vector<std::shared_ptr<T>> children;
};
}
