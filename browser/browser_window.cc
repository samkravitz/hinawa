#include "browser_window.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "css.h"
#include "document/element.h"
#include "document/text.h"
#include "html/parser.h"
#include "js/vm.h"
#include "layout/block.h"
#include "layout/image.h"
#include "layout/node.h"
#include "layout/rect.h"
#include "web/resource.h"

#include "SkCanvas.h"
#include "SkFont.h"
#include "SkFontMgr.h"
#include "SkFontStyle.h"
#include "SkPath.h"
#include "SkRRect.h"
#include "SkSurface.h"

fs::path DATA_DIR = HINAWA_DATA_DIR;

namespace browser
{
BrowserWindow::BrowserWindow(const Url &u) :
    url(u)
{
	QMainWindow::resize(width, height);
	label.setParent(this);
	setMouseTracking(true);
	label.setMouseTracking(true);

	css::read_properties_file();
	load(url);

	layout::Box viewport;
	viewport.content.width = width;
	viewport.content.height = 0;
	layout_tree->layout(viewport);
	layout::print_tree_with_lines(layout_tree.get());

	resize();
	render();
	show();
}

void BrowserWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);
	auto size = event->size();
	width = size.width();
	height = size.height();

	resize();
	layout::Box viewport = {};
	viewport.content.width = width;
	viewport.content.height = 0;
	layout_tree->layout(viewport);
	layout_tree->print("Layout Tree");
	render();
}

void BrowserWindow::mouseMoveEvent(QMouseEvent *event)
{
	QMainWindow::mouseMoveEvent(event);
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

void BrowserWindow::mousePressEvent(QMouseEvent *event)
{
	QMainWindow::mousePressEvent(event);

	if (event->button() != Qt::LeftButton)
		return;

	auto pos = event->pos();

	Point p = {pos.x(), pos.y()};
	if (!hovered_href.empty())
	{
		auto new_url = Url(hovered_href, &url);
		load(new_url);
		layout::Box viewport = {};
		viewport.content.width = width;
		viewport.content.height = 0;
		layout_tree->layout(viewport);
		layout_tree->print("Layout Tree");
		render();
	}

	if (document.show_alert() && alert_box.contains(p))
	{
		document.clear_alert();
		render();
	}
}

void BrowserWindow::load(const Url &new_url)
{
	url = new_url;
	::load(url, [&](const auto &data) {
		document = Document(url);
		auto parser = html::Parser(document);
		document = parser.parse(std::string((const char *) data.data(), data.size()));
		document.print("Document");

		style_tree = css::build_style_tree(document);
		layout_tree = layout::build_layout_tree(style_tree.get());

		auto javascript = document.get_script();
		if (javascript == "")
			return;

		js::Vm vm(&document);
		vm.interpret(javascript);
	});
}

void BrowserWindow::resize()
{
	auto info = SkImageInfo::MakeN32Premul(width, height);
	auto row_bytes = info.minRowBytes();
	auto size = info.computeByteSize(row_bytes);
	pixels.reserve(size);
	surface = SkSurfaces::WrapPixels(info, &pixels[0], row_bytes);
	label.resize(width, height);
}

void BrowserWindow::raster()
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

void BrowserWindow::render()
{
	raster();
	image = QImage(pixels.data(), width, height, QImage::Format::Format_RGB32);
	label.setPixmap(QPixmap::fromImage(image));
	label.show();
}
}