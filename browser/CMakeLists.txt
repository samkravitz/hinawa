set(SOURCES
	browser_window.cc
	web_view.cc
)

set(HEADERS
	browser_window.h
	web_view.h
)

target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
