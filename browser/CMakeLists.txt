set(SOURCES
	browser.cc
	browser_window.cc
)

set(browser
	browser.h
	browser_window.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(hinawa.test PRIVATE ..)
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
