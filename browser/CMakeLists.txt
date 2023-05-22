set(SOURCES
	browser.cc
)

set(browser
	browser.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(hinawa.test PRIVATE ..)
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
