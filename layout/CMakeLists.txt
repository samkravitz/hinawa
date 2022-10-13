set(SOURCES
	block.cc
	inline.cc
	node.cc
	text.cc
	layout_node.cc
)

set(HEADERS
	block.h
	coordinate.h
	box.h
	edges.h
	inline.h
	node.h
	layout_node.h
	text.h
	rect.h
)

target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
