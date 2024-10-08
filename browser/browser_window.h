#pragma once

#include "web_view.h"

#include <QLineEdit>
#include <QMainWindow>
#include <QResizeEvent>

namespace browser
{
class BrowserWindow : public QMainWindow
{
	Q_OBJECT

public:
	BrowserWindow(const Url &);

private:
	WebView *view = nullptr;

	// width, height of window in px
	int width = 1600;
	int height = 1200;

	void render();
	QLineEdit url_bar;

	virtual void resizeEvent(QResizeEvent *) override;

private slots:
	void url_selected();
};
}