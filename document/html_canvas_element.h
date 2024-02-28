#pragma once

#include "element.h"
#include "util/hinawa.h"

#include <memory>
#include <vector>

#include "SkBitmap.h"

class CanvasRenderingContext2D;

class HtmlCanvasElement final : public Element
{
public:
	HtmlCanvasElement(Document &document, std::string tag) :
	    Element(document, tag)
	{ }

	float width() const;
	float height() const;
	SkBitmap bitmap() const { return m_bitmap; }

	CanvasRenderingContext2D *get_context(std::string);

private:
	SkBitmap m_bitmap;
	CanvasRenderingContext2D *m_context = nullptr;
};
