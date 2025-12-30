set(SOURCES
	painter.cc
	font_mgr.cc
)

set(HEADERS
	color.h
	painter.h
	point.h
	font_mgr.h
)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
