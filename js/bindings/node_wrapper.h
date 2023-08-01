#include "wrapper.h"

class Node;

namespace js
{
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
}
}
