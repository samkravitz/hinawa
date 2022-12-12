set(SOURCES
	url_parser.cc
	url.cc
)

set(HEADERS
	url_parser.h
	url.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
