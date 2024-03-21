#include "browser_window.h"

fs::path DATA_DIR = HINAWA_DATA_DIR;

static constexpr int NAVBAR_HEIGHT = 28;

namespace browser
{
BrowserWindow::BrowserWindow(const Url &url)
{
	QMainWindow::resize(width, height);

	toolbar.setFixedHeight(NAVBAR_HEIGHT);
	url_bar.setParent(&toolbar);
	url_bar.setText(QString::fromStdString(url.serialize()));
	toolbar.addWidget(&url_bar);
	toolbar.setFloatable(false);
	toolbar.setMovable(false);
	QMainWindow::addToolBar(&toolbar);

	view = new WebView(url, width, height);
	view->setGeometry(0, NAVBAR_HEIGHT, width, height - NAVBAR_HEIGHT);
	view->setParent(this);
	view->resize(width, height);
	view->setFocus();

	QObject::connect(view, &WebView::load_started, &url_bar, &QLineEdit::setText);
	QObject::connect(&url_bar, &QLineEdit::returnPressed, this, &BrowserWindow::url_selected);

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

void BrowserWindow::url_selected()
{
	view->url_selected(url_bar.text().toStdString());
}
}