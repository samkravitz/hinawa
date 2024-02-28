if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options("-DDEBUG_PRINT_CODE")
	add_compile_options("-DDEBUG_PRINT_STACK")
	add_compile_options("-DDEBUG_PRINT_AST")
	#add_compile_options("-DDEBUG_LOG_GC")
endif()

set(SOURCES
	bindings/canvas_rendering_context_2d_wrapper.cc
	bindings/document_wrapper.cc
	bindings/html_canvas_element_wrapper.cc
	bindings/node_wrapper.cc

	array.cc
	chunk.cc
	compiler.cc
	date.cc
	error.cc
	function.cc
	heap.cc
	object_string.cc
	object.cc
	parser.cc
	prelude.cc
	scanner.cc
	token.cc
	value.cc
	vm.cc

	../document/canvas_rendering_context_2d.cc
	../document/html_canvas_element.cc
	../document/document.cc
	../document/node.cc
	../document/element.cc
	../document/text.cc
)

set(HEADERS
	ast/ast.h
	ast/expr.h
	ast/stmt.h
	ast/visitor.h

	bindings/canvas_rendering_context_2d_wrapper.h
	bindings/document_wrapper.h
	bindings/html_canvas_element_wrapper.h
	bindings/node_wrapper.h
	bindings/wrapper.h

	array.h
	cell.h
	chunk.h
	compiler.h
	date.h
	error.h
	function.h
	heap.h
	object_string.h
	object.h
	opcode.h
	operator.h
	parser.h
	prelude.h
	scanner.h
	string.hh
	token_type.h
	token.h
	value.h
	vm.h

	../document/canvas_rendering_context_2d.h
	../document/document.h
	../document/node.h
	../document/element.h
	../document/text.h
)

flex_target(JsLexer2 lex.ll lex.yy.cc)

add_executable(js main.cc ${SOURCES} ${HEADERS} ${FLEX_JsLexer2_OUTPUTS})

target_include_directories(js
	PRIVATE
	.
	..
	${Skia_BUILD_DIR}
	${Skia_INCLUDE_DIR}
	${Skia_INCLUDE_DIR}/core
)
target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_link_libraries(js fmt::fmt)

target_include_directories(hinawa.test PRIVATE ..)
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
