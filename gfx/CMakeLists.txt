set(SOURCES
	painter.cc
)

set(HEADERS
	color.h
	painter.h
	point.h
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
