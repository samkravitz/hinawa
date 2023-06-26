#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace util
{
template<class T> class TreeNode
{
public:
	TreeNode() = default;
	virtual ~TreeNode() = default;

	virtual std::string to_string() const { return "TREE BASE"; };

	void add_child(const std::shared_ptr<T> &node)
	{
		node->m_parent = static_cast<T *>(this);
		children.push_back(node);
	}

	void prepend_child(const std::shared_ptr<T> &node)
	{
		node->m_parent = static_cast<T *>(this);
		children.insert(children.begin(), 1, node);
	}

	T *parent() const { return m_parent; }

	void preorder(std::function<void(T *)> f)
	{
		auto *p = static_cast<T *>(this);
		f(p);

		for (auto child : children)
			child->preorder(f);
	}

	void postorder(std::function<void(T *)> f)
	{
		for (auto child : children)
			child->postorder(f);

		auto *p = static_cast<T *>(this);
		f(p);
	}

	void for_each_child(std::function<void(T *)> f)
	{
		for (auto child : children)
			f(child.get());
	}

	bool has_child_that_matches_condition(std::function<bool(const T *)> f) const
	{
		for (const auto &child : children)
		{
			if (f(child.get()))
				return true;

			return child->has_child_that_matches_condition(f);
		}

		return false;
	}

	bool parent_matches_condition(std::function<bool(const T *)> f) const
	{
		if (!parent())
			return false;

		return f(parent());
	}

	T *last_child()
	{
		if (!has_children())
			return nullptr;

		return children.back().get();
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
