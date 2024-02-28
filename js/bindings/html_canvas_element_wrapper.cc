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

	set_native_property(
	    "width",
	    [canvas](Object *) { return Value(canvas->width()); },
	    [canvas](Object *, Value value) { canvas->set_width(value.as_number()); });

	set_native_property(
	    "height",
	    [canvas](Object *) { return Value(canvas->height()); },
	    [canvas](Object *, Value value) { canvas->set_height(value.as_number()); });
}

HtmlCanvasElement &HtmlCanvasElementWrapper::node()
{
	return static_cast<HtmlCanvasElement &>(NodeWrapper::node());
}
}
}
