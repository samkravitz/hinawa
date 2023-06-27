set(SOURCES
	browser_window.cc
	painter.cc
)

set(HEADERS
	browser_window.h
	painter.h
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
