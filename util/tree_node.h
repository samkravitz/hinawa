#pragma once

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

	void add_child(std::shared_ptr<T> node) { children.push_back(node); }

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

protected:
	std::vector<std::shared_ptr<T>> children;
};
}
