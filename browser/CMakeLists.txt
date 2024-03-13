set(SOURCES
	browser_window.cc
)

set(HEADERS
	browser_window.h
)

target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
