#pragma once

#include "element.h"
#include "util/hinawa.h"

#include <memory>
#include <vector>

#include "SkCanvas.h"
#include "SkSurface.h"

class CanvasRenderingContext2D;

class HtmlCanvasElement final : public Element
{
public:
	HtmlCanvasElement(Document &document, std::string tag) :
	    Element(document, tag)
	{ }

	void add_attribute(std::string, std::string) override;
	float width() const;
	float height() const;
	SkCanvas &canvas() const { return *m_surface->getCanvas(); }

	CanvasRenderingContext2D *get_context(std::string);

private:
	sk_sp<SkSurface> m_surface;
	std::vector<unsigned char> m_pixels;

	CanvasRenderingContext2D *m_context = nullptr;

	static constexpr int PREFERRED_WIDTH = 300;
	static constexpr int PREFERRED_HEIGHT = 300;

	float m_width = 0;
	float m_height = 0;
};
