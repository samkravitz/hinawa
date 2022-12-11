#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>
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
		node->m_parent = static_cast<T *>(this);
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

	void print(std::string const &title = "")
	{
		if (!title.empty())
			std::cout << title << "\n";

		std::cout << to_string() << "\n";
		print_subtree("");
	}

	void pop_back()
	{
		auto x = last_child();
		for (x; x->has_children(); x = x->last_child())
			;
		
		x->parent()->pop();
	}

	void pop()
	{
		children.pop_back();
	}

protected:
	T *m_parent = nullptr;
	std::vector<std::shared_ptr<T>> children;

private:
	void print_subtree(std::string const &prefix)
	{
		if (!has_children())
			return;

		std::cout << prefix;
		if (children.size() > 1)
			std::cout << "├──";

		for (unsigned i = 0; i < children.size(); i++)
		{
			auto child = children[i];
			if (i < children.size() - 1)
			{
				if (i > 0)
					std::cout << prefix << "├──";

				auto new_prefix = prefix;
				if (children.size() > 1 && !child->children.empty())
					new_prefix += "│";
				new_prefix += "    ";

				std::cout << child->to_string() << "\n";
				child->print_subtree(new_prefix);
			}

			else
			{
				if (children.size() > 1)
					std::cout << prefix;

				std::cout << "└──";
				std::cout << child->to_string() << "\n";
				child->print_subtree(prefix + "    ");
			}
		}
	}
};
}
