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

target_include_directories(hinawa
	PRIVATE
	..
	${Skia_BUILD_DIR}
	${Skia_INCLUDE_DIR}
	${Skia_INCLUDE_DIR}/core
)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(hinawa.test
	PRIVATE
	..
	${Skia_BUILD_DIR}
	${Skia_INCLUDE_DIR}
	${Skia_INCLUDE_DIR}/core
)
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})