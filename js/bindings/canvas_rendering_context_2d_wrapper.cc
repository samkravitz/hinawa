#include "canvas_rendering_context_2d_wrapper.h"

#include "document/canvas_rendering_context_2d.h"
#include "heap.h"
#include "object_string.h"

#include <string>

namespace js
{
namespace bindings
{
CanvasRenderingContext2DWrapper::CanvasRenderingContext2DWrapper(CanvasRenderingContext2D *context) :
    m_context(context)
{
	set_native_property(
	    "fillStyle",
	    [this](Object *) { return Value(heap().allocate_string(m_context->fill_style())); },
	    [this](Object *, Value value) { m_context->set_fill_style(value.to_string()); });

	set_native("fillRect", [this](auto &vm, const auto &argv) -> Value {
		if (argv.size() < 4)
			return Value::js_undefined();

		m_context->fill_rect(argv[0].as_number(), argv[1].as_number(), argv[2].as_number(), argv[3].as_number());

		return Value::js_undefined();
	});
}
}
}
