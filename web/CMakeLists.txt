set(SOURCES
	http.cc
	url_parser.cc
	url.cc
)

set(HEADERS
	http.h
	url_parser.h
	url.h
)

add_executable(web main.cc ${SOURCES} ${HEADERS})

target_include_directories(web PRIVATE ..)
target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_link_libraries(web ssl crypto)
