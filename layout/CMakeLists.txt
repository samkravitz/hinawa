set(SOURCES
	layout_node.cc
)

set(HEADERS
	box.h
	layout_node.h
	rect.h
)

target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
