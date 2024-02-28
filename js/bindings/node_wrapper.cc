#include "node_wrapper.h"

#include "document/document.h"
#include "document/element.h"
#include "document/html_canvas_element.h"
#include "document_wrapper.h"
#include "heap.h"
#include "html_canvas_element_wrapper.h"
#include "object_string.h"

#include <string>

namespace js
{
namespace bindings
{
NodeWrapper::NodeWrapper(Node *node) :
    m_node(node)
{
	auto *js_string = heap().allocate_string(node->element_name());
	set("nodeName", Value(js_string));
}

NodeWrapper *wrap(js::Heap &heap, Node &node)
{
	if (dynamic_cast<HtmlCanvasElement *>(&node))
		return heap.allocate<HtmlCanvasElementWrapper>(static_cast<HtmlCanvasElement *>(&node));
	return heap.allocate<NodeWrapper>(&node);
}
}
}
