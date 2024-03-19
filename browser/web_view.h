#pragma once

#include "css/styled_node.h"
#include "document/document.h"
#include "layout/node.h"
#include "layout/rect.h"
#include "web/url.h"

#include <QLabel>
#include <QMouseEvent>

#include "SkSurface.h"

namespace browser
{
class WebView : public QLabel
{
	Q_OBJECT

public:
	WebView(const Url &, int width, int height);

	void resize(int, int);
	void render();

private:
	Url url;
	Document document;
	std::shared_ptr<css::StyledNode> style_tree;
	std::shared_ptr<layout::Node> layout_tree;

	// href of the current hovered link
	std::string hovered_href = "";

	QLabel label;
	QImage image;
	std::vector<unsigned char> pixels;
	sk_sp<SkSurface> surface;

	// width, height of view in px
	int width;
	int height;

	layout::Rect alert_box;

	void load(const Url &);
	void raster();
	void layout();

	virtual void mouseMoveEvent(QMouseEvent *) override;
	virtual void mousePressEvent(QMouseEvent *) override;
};
}
