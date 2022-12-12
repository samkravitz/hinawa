set(SOURCES
	window.cc
)

set(HEADERS
	window.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
