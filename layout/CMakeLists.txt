set(SOURCES
	block.cc
	image.cc
	inline.cc
	node.cc
	text.cc
)

set(HEADERS
	block.h
	box.h
	edges.h
	image.h
	inline.h
	node.h
	text.h
	rect.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
