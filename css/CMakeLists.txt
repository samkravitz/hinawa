set(SOURCES
	css.cc
	parser.cc
	scanner.cc
	selector.cc
	styled_node.cc
	stylesheet.cc
	token.cc
)

set(HEADERS
	component_value.h
	css.h
	parser.h
	scanner.h
	selector.h
	styled_node.h
	stylesheet.h
	token.h
	value.h
)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
