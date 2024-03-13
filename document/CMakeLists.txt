set(SOURCES
	canvas_rendering_context_2d.cc
	document.cc
	element.cc
	element_factory.cc
	html_canvas_element.cc
	html_image_element.cc
	node.cc
	text.cc
)

set(HEADERS
	canvas_rendering_context_2d.h
	document.h
	element.h
	element_factory.h
	html_canvas_element.h
	html_image_element.h
	node.h
	text.h
)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
