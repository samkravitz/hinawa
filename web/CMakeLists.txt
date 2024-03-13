set(SOURCES
	http.cc
	resource.cc
	url_parser.cc
	url.cc
)

set(HEADERS
	http.h
	resource.h
	url_parser.h
	url.h
)

add_executable(web main.cc ${SOURCES} ${HEADERS})

target_include_directories(web PRIVATE ..)
target_link_libraries(web curl)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})

