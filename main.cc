#include <curl/curl.h>
#include <filesystem>
#include <string>

#include "browser/browser.h"
#include "util/hinawa.h"

//static const char* homepage_url = "index.html";

//int main(int argc, char** argv)
//{
//	std::string html_file = DATA_DIR / "homepage" / homepage_url;
//	if (argc > 1)
//	{
//		html_file = argv[1];
//	}

//	Url url;
//	if (fs::exists(html_file))
//		url = Url("file://" + fs::canonical(html_file).string());

//	else
//		url = Url(html_file);

//	curl_global_init(CURL_GLOBAL_ALL);
//	auto hinawa = browser::Browser(url);
//	curl_global_cleanup();
//	return 0;
//}

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/application.h>

#include <iostream>

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld()
: m_button("Hello World")   // creates a new button with label "Hello World".
{
  // Sets the margin around the button.
  m_button.set_margin(10);

  // When the button receives the "clicked" signal, it will call the
  // on_button_clicked() method defined below.
  m_button.signal_clicked().connect(sigc::mem_fun(*this,
              &HelloWorld::on_button_clicked));

  // This packs the button into the Window (a container).
  set_child(m_button);
}
~HelloWorld()
{
}


protected:
  //Signal handlers:
  void on_button_clicked(){
  std::cout << "Hello World" << std::endl;
}

  //Member widgets:
  Gtk::Button m_button;
};

int main(int argc, char* argv[])
{
  auto app = Gtk::Application::create("org.gtkmm.example");

  //Shows the window and returns when it is closed.
  return app->make_window_and_run<HelloWorld>(argc, argv);
}