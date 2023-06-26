#include "browser_window.h"

namespace browser
{
BrowserWindow::BrowserWindow(const Url &u) :
    url(u)
{
	label.show();
}
}