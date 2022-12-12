set(SOURCES
	parser.cc
	scanner.cc
	styled_node.cc
	stylesheet.cc
	token.cc
)

set(HEADERS
	parser.h
	scanner.h
	styled_node.h
	stylesheet.h
	token.h
	value.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
