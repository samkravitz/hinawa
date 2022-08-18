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

target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
