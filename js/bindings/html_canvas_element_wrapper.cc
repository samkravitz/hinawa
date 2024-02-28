#include "html_canvas_element_wrapper.h"

#include "canvas_rendering_context_2d_wrapper.h"
#include "document/canvas_rendering_context_2d.h"
#include "document/html_canvas_element.h"
#include "heap.h"
#include "object_string.h"

#include <string>

namespace js
{
namespace bindings
{
HtmlCanvasElementWrapper::HtmlCanvasElementWrapper(HtmlCanvasElement *canvas) :
    NodeWrapper(canvas)
{
	set_native("getContext", [this](auto &vm, const auto &argv) -> Value {
		auto *context = node().get_context("2d");
		return Value(heap().allocate<CanvasRenderingContext2DWrapper>(context));
	});
}

HtmlCanvasElement &HtmlCanvasElementWrapper::node()
{
	return static_cast<HtmlCanvasElement &>(NodeWrapper::node());
}
}
}
