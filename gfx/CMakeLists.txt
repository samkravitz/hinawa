set(SOURCES
	painter.cc
)

set(HEADERS
	color.h
	painter.h
	point.h
)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
