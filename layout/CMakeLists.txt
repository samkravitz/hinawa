set(SOURCES
	block.cc
	inline.cc
	node.cc
	text.cc
)

set(HEADERS
	block.h
	coordinate.h
	box.h
	edges.h
	inline.h
	node.h
	text.h
	rect.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
