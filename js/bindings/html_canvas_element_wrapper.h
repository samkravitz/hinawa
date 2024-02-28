#pragma once

#include "node_wrapper.h"

class HtmlCanvasElement;

namespace js
{
namespace bindings
{
class HtmlCanvasElementWrapper : public NodeWrapper
{
public:
	HtmlCanvasElementWrapper(HtmlCanvasElement *);

	HtmlCanvasElement &node();
};
}
}
