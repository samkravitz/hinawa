#pragma once

#include "element.h"
#include "util/hinawa.h"

#include <vector>

#include "SkBitmap.h"

class HtmlImageElement final : public Element
{
public:
	HtmlImageElement(Document &document, std::string tag) :
	    Element(document, tag)
	{ }

	void add_attribute(std::string, std::string) override;
	float width() const;
	float height() const;
	SkBitmap bitmap() const { return m_bitmap; }

private:
	SkBitmap m_bitmap;
	std::vector<u8> m_data;
};
