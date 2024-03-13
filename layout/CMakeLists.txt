set(SOURCES
	block.cc
	canvas.cc
	image.cc
	inline.cc
	list.cc
	node.cc
	text.cc
)

set(HEADERS
	block.h
	box.h
	canvas.h
	edges.h
	image.h
	inline.h
	list.h
	node.h
	text.h
	rect.h
)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
