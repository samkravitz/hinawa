#include "web_view.h"

#include "css.h"
#include "document/element.h"
#include "html/parser.h"
#include "js/vm.h"
#include "layout/node.h"
#include "layout/rect.h"
#include "web/resource.h"

#include "SkCanvas.h"
#include "SkFont.h"
#include "SkFontMgr.h"
#include "SkFontStyle.h"
#include "SkSurface.h"

namespace browser
{
WebView::WebView(const Url &u, int initial_width, int initial_height) :
    url(u),
    width(initial_width),
    height(initial_height)
{
	setMouseTracking(true);

	css::read_properties_file();
	load(url);

	// initial layout prints the layout tree with line fragment information,
	// whereas other layouts don't
	layout::Box viewport;
	viewport.content.width = width;
	viewport.content.height = 0;
	layout_tree->layout(viewport);
	layout::print_tree_with_lines(layout_tree.get());
}

void WebView::raster()
{
	auto *canvas = surface->getCanvas();
	gfx::Painter painter(canvas, width, height);

	/**
	 * Before rendering, paint the entire canvas white.
	 * Here we will also check for an edge condition, where the background property of the <body>
	 * element is set. If it is, instead of just covering the dimensions that the body element
	 * takes up, the entire canvas is to be covered with that background color
	 * 
	 * @ref https://www.w3.org/TR/css-backgrounds-3/#special-backgrounds
	*/
	painter.fill_rect(Color::WHITE);

	layout_tree->preorder([&painter](auto const &layout_node) { layout_node->render(painter); });

	if (document.show_alert())
	{
		constexpr int ALERT_BOX_WIDTH = 400;
		constexpr int ALERT_BOX_HEIGHT = 250;
		constexpr int ALERT_BOX_Y = 4;
		constexpr int ALERT_FONT_SIZE = 24;

		alert_box.x = (width - ALERT_BOX_WIDTH) / 2.0;
		alert_box.y = ALERT_BOX_Y;
		alert_box.width = ALERT_BOX_WIDTH;
		alert_box.height = ALERT_BOX_HEIGHT;

		layout::Rect rect = {};
		auto gray = Color(0xa6, 0xa6, 0xa6);
		rect.set_position(alert_box.x, alert_box.y);
		rect.set_size(alert_box.width, alert_box.height);
		painter.fill_rect(rect, gray);

		auto alert_text = document.alert_text();
		SkFontStyle font_style;
		auto font_mgr = SkFontMgr::RefDefault();
		auto typeface = font_mgr->legacyMakeTypeface(nullptr, font_style);
		auto font = SkFont(typeface, ALERT_FONT_SIZE);
		painter.draw_text(alert_text, font, alert_box.x + 8, (ALERT_BOX_HEIGHT + ALERT_BOX_Y) / 2.0, Color::WHITE);
	}
}

void WebView::render()
{
	raster();
	image = QImage(pixels.data(), width, height, QImage::Format::Format_RGB32);
	setPixmap(QPixmap::fromImage(image));
	show();
}

void WebView::load(const Url &new_url)
{
	url = new_url;
	document = Document(url);
	emit load_started(QString::fromStdString(url.serialize()));
	::load(url, [&](const auto &data) {
		html::Parser::parse(std::string((const char *) data.data(), data.size()), document);
		document.print("Document");

		style_tree = css::build_style_tree(document);
		layout_tree = layout::build_layout_tree(style_tree.get());
	});
}

void WebView::resize(int w, int h)
{
	QLabel::resize(w, h);
	width = w;
	height = h;
	layout();

	auto info = SkImageInfo::MakeN32Premul(width, height);
	auto row_bytes = info.minRowBytes();
	auto size = info.computeByteSize(row_bytes);
	pixels.reserve(size);
	surface = SkSurfaces::WrapPixels(info, &pixels[0], row_bytes);
}

void WebView::layout()
{
	layout::Box viewport = {};
	viewport.content.width = width;
	viewport.content.height = 0;
	layout_tree->layout(viewport);
	layout_tree->print("Layout Tree");
}

void WebView::mouseMoveEvent(QMouseEvent *event)
{
	QLabel::mouseMoveEvent(event);
	auto pos = event->pos();

	auto result = layout_tree->hit_test(Point{pos.x(), pos.y()});
	if (result && result.value()->is_link())
	{
		setCursor(Qt::PointingHandCursor);

		// grabs the most specific node (Text), when we want the Element to see the href
		auto *element = dynamic_cast<Element *>(result.value()->parent());
		auto href = element->get_attribute("href");
		hovered_href = href;
	}

	else
	{
		setCursor(Qt::ArrowCursor);
		hovered_href = "";
	}
}

void WebView::mousePressEvent(QMouseEvent *event)
{
	QLabel::mousePressEvent(event);

	if (event->button() != Qt::LeftButton)
		return;

	auto pos = event->pos();

	Point p = {pos.x(), pos.y()};
	if (!hovered_href.empty())
	{
		const std::string javascript_url = "javascript:";
		auto is_javascript_url = [&javascript_url](const std::string &str) { return str.starts_with(javascript_url); };
		if (is_javascript_url(hovered_href))
		{
			auto source = hovered_href.substr(javascript_url.size());
			document.vm().interpret(source);
			render();
		}

		else
		{
			auto new_url = Url(hovered_href, &url);
			load(new_url);
			layout();
			render();
		}
	}

	if (document.show_alert() && alert_box.contains(p))
	{
		document.clear_alert();
		render();
	}
}

void WebView::url_selected(const std::string &new_url)
{
	load(Url(new_url));
	layout();
	render();
}
}
