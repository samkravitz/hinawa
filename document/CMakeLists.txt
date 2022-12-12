set(SOURCES
	dom.cc
	element.cc
	node.cc
	text.cc
)

set(HEADERS
	dom.h
	element.h
	node.h
	text.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
