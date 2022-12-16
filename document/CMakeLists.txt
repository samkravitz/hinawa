set(SOURCES
	document.cc
	element.cc
	node.cc
	text.cc
)

set(HEADERS
	document.h
	element.h
	node.h
	text.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
