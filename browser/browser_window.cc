#include "browser_window.h"

fs::path DATA_DIR = HINAWA_DATA_DIR;

namespace browser
{
BrowserWindow::BrowserWindow(const Url &url)
{
	QMainWindow::resize(width, height);

	view = new WebView(url, width, height);
	view->setParent(this);
	view->resize(width, height);
	render();

	QMainWindow::show();
}

void BrowserWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);
	auto size = event->size();
	width = size.width();
	height = size.height();

	view->resize(width, height);
	render();
}

void BrowserWindow::render()
{
	view->render();
}
}