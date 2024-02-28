#include "document_wrapper.h"

#include <string>

#include "document/element.h"
#include "document/html_canvas_element.h"
#include "heap.h"
#include "node_wrapper.h"

namespace js
{
namespace bindings
{
DocumentWrapper::DocumentWrapper(Document *doc) :
    m_document(doc)
{
	set_native("getElementById", [this](auto &vm, const auto &argv) -> Value {
		if (argv.empty())
			return Value::js_null();

		auto id = argv[0].to_string();
		auto *node = static_cast<Node *>(document().get_element_by_id(id));
		if (!node)
			return Value::js_null();

		return Value(wrap(heap(), *node));
	});
}
}
}
