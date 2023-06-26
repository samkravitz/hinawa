#include "browser_window.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "css.h"
#include "document/element.h"
#include "document/text.h"
#include "html/parser.h"
#include "js/compiler.h"
#include "js/parser.h"
#include "js/vm.h"
#include "layout/block.h"
#include "layout/image.h"
#include "layout/node.h"
#include "web/resource.h"

#include "SkCanvas.h"
#include "SkPath.h"
#include "SkRRect.h"
#include "SkSurface.h"

namespace browser
{
BrowserWindow::BrowserWindow(const Url &u) :
    url(u)
{
	css::read_properties_file();
	load(url);

	layout::Box viewport;
	viewport.content.width = width;
	viewport.content.height = 0;
	layout_tree->layout(viewport);
	layout::print_tree_with_lines(layout_tree.get());

	render();
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

		auto js_parser = js::Parser(javascript);
		auto program = js_parser.parse();
		js::Compiler compiler{program};
		auto fn = compiler.compile();
		js::Vm vm(&document);
		vm.run(std::move(fn));
	});
}

std::vector<char> BrowserWindow::raster(int width, int height, void (*draw)(SkCanvas *))
{
	SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
	auto row_bytes = info.minRowBytes();
	auto size = info.computeByteSize(row_bytes);
	std::vector<char> pixels(size);
	sk_sp<SkSurface> surface = SkSurfaces::WrapPixels(info, &pixels[0], row_bytes);
	auto *canvas = surface->getCanvas();
	draw(canvas);
	return pixels;
}

void draw(SkCanvas *canvas)
{
	canvas->drawColor(SK_ColorWHITE);

	SkPaint paint;
	paint.setStyle(SkPaint::kFill_Style);
	paint.setAntiAlias(true);
	paint.setStrokeWidth(4);
	paint.setColor(0xff4285F4);

	SkRect rect = SkRect::MakeXYWH(10, 10, 100, 160);
	canvas->drawRect(rect, paint);

	SkRRect oval;
	oval.setOval(rect);
	oval.offset(40, 80);
	paint.setColor(0xffDB4437);
	canvas->drawRRect(oval, paint);

	paint.setColor(0xff0F9D58);
	canvas->drawCircle(180, 50, 25, paint);

	rect.offset(80, 50);
	paint.setColor(0xffF4B400);
	paint.setStyle(SkPaint::kStroke_Style);
	canvas->drawRoundRect(rect, 10, 10, paint);
}

void BrowserWindow::render()
{
	data = raster(width, height, draw);
	image = QImage((const uchar *) data.data(), width, height, QImage::Format::Format_RGB32);
	label.setPixmap(QPixmap::fromImage(image));
	label.show();
}
}