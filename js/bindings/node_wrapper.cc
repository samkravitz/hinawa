#include "node_wrapper.h"

#include "document/element.h"

#include <string>

namespace js
{
namespace bindings
{
NodeWrapper::NodeWrapper(Node *node) :
    m_node(node)
{
	auto *js_string = new std::string(node->element_name());
	set("nodeName", Value(js_string));
}
}
}
