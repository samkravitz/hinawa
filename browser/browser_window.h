#pragma once

#include "css/styled_node.h"
#include "document/document.h"
#include "layout/node.h"
#include "layout/rect.h"
#include "web/url.h"

#include <QLabel>
#include <QMainWindow>

#include "include/core/SkCanvas.h"
#include "include/core/SkRRect.h"
#include "include/core/SkSurface.h"
#include "include/core/SkPath.h"

namespace browser
{
class BrowserWindow : public QMainWindow
{
	Q_OBJECT

public:
	BrowserWindow(const Url &);

private:
	Url url;
	Document document;
	std::shared_ptr<css::StyledNode> style_tree;
	std::shared_ptr<layout::Node> layout_tree;

	// href of the current hovered link
	std::string hovered_href = "";

	QLabel label;
	QImage image;
	std::vector<char> data;

	// width, height of window in px
	int width = 1600;
	int height = 1200;

	layout::Rect alert_box;

	void load(const Url &);
	void render();
	std::vector<char> raster(int, int, void (*draw)(SkCanvas*));
};
}