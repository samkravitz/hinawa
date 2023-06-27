#pragma once

#include "element.h"
#include "util/hinawa.h"

#include <vector>

#include "SkBitmap.h"
#include "SkImageInfo.h"

class HtmlImageElement final : public Element
{
public:
	HtmlImageElement(Document &document, std::string tag) :
	    Element(document, tag)
	{ }

	void add_attribute(std::string, std::string) override;
	float width() const;
	float height() const;
	SkImageInfo image() const { return m_image; }
	SkBitmap bitmap() const { return m_bitmap; }

private:
	SkImageInfo m_image;
	SkBitmap m_bitmap;
	std::vector<u8> m_data;
};
