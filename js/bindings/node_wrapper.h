#pragma once

#include "wrapper.h"

class Node;

namespace js
{
class Heap;

namespace bindings
{
class NodeWrapper : public Wrapper
{
public:
	NodeWrapper(Node *);

	Node &node() const { return *m_node; }
private:
	Node *m_node = nullptr;
};

NodeWrapper *wrap(js::Heap &, Node &);
}
}
