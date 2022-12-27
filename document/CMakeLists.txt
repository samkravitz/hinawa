set(SOURCES
	document.cc
	element.cc
	element_factory.cc
	html_image_element.cc
	node.cc
	text.cc
)

set(HEADERS
	document.h
	element.h
	element_factory.h
	html_image_element.h
	node.h
	text.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
