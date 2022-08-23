set(SOURCES
	layout_node.cc
)

set(HEADERS
	coordinate.h
	box.h
	edges.h
	layout_node.h
	rect.h
)

target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
