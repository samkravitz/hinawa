#include "browser_window.h"

#include <QAction>
#include <QMenuBar>
#include <QToolBar>

fs::path DATA_DIR = HINAWA_DATA_DIR;

static constexpr int NAVBAR_HEIGHT = 28;

namespace browser
{
BrowserWindow::BrowserWindow(const Url &url)
{
	QMainWindow::resize(width, height);

	auto *menu = menuBar()->addMenu("&File");
	const auto menu_height = menu->height();

	auto *quit_action = new QAction("&Quit", this);
	menu->addAction(quit_action);

	auto *toolbar = addToolBar("main toolbar");
	url_bar.setParent(this);
	url_bar.setText(QString::fromStdString(url.serialize()));
	toolbar->addWidget(&url_bar);
	toolbar->setFloatable(false);
	toolbar->setMovable(false);
	QMainWindow::addToolBar(toolbar);

	view = new WebView(url, width, height);
	view->setGeometry(0, NAVBAR_HEIGHT + menu_height, width, height - menu_height);
	view->setParent(this);
	view->resize(width, height);
	view->setFocus();

	QObject::connect(view, &WebView::load_started, &url_bar, &QLineEdit::setText);
	QObject::connect(&url_bar, &QLineEdit::returnPressed, this, &BrowserWindow::url_selected);
	QObject::connect(quit_action, &QAction::triggered, this, &QMainWindow::close);

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