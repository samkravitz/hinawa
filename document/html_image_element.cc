#include "html_image_element.h"

#include "web/resource.h"

#include <fmt/format.h>

#include "SkImageInfo.h"
#include <QImage>

void HtmlImageElement::add_attribute(std::string name, std::string value)
{
	if (name == "src")
	{
		auto origin = document().origin();
		load({value, &origin}, [this](const auto &data) {
			QImage image;
			if (!image.loadFromData(data.data(), data.size()))
			{
				fmt::print(stderr, "Error decoding bitmap\n");
				return;
			}

			m_image = SkImageInfo::Make(image.width(), image.height(), kBGRA_8888_SkColorType, kPremul_SkAlphaType);
			m_bitmap.setInfo(m_image);
			m_data = std::vector<u8>((u8 *) image.bits(), image.bits() + image.sizeInBytes());
			m_bitmap.setPixels((void *) m_data.data());

			document().set_needs_reflow();
		});
	}

	Element::add_attribute(name, value);
}

float HtmlImageElement::width() const
{
	return m_image.width();
}

float HtmlImageElement::height() const
{
	return m_image.height();
}
