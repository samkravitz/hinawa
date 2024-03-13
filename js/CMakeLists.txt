cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(js DESCRIPTION "JavaScript Runtime")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options("-DDEBUG_PRINT_CODE")
	add_compile_options("-DDEBUG_PRINT_STACK")
	add_compile_options("-DDEBUG_PRINT_AST")
	#add_compile_options("-DDEBUG_LOG_GC")
endif()

add_library(libjs STATIC)

find_package(fmt REQUIRED)
find_package(FLEX REQUIRED)

set(JS_BINDINGS_SOURCES
	js/bindings/canvas_rendering_context_2d_wrapper.cc
	js/bindings/document_wrapper.cc
	js/bindings/html_canvas_element_wrapper.cc
	js/bindings/node_wrapper.cc

	js/../document/canvas_rendering_context_2d.cc
	js/../document/html_canvas_element.cc
	js/../document/document.cc
	js/../document/node.cc
	js/../document/element.cc
	js/../document/text.cc

	PARENT_SCOPE
)

set(JS_BINDINGS_HEADERS
	js/bindings/canvas_rendering_context_2d_wrapper.h
	js/bindings/document_wrapper.h
	js/bindings/html_canvas_element_wrapper.h
	js/bindings/node_wrapper.h
	js/bindings/wrapper.h

	js/../document/canvas_rendering_context_2d.h
	js/../document/document.h
	js/../document/node.h
	js/../document/element.h
	js/../document/text.h

	PARENT_SCOPE
)

set(SOURCES
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
)

set(HEADERS
	ast/ast.h
	ast/expr.h
	ast/stmt.h
	ast/visitor.h

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
)

flex_target(JsLexer lex.ll lex.yy.cc)

set(
	JS_FLEX_OUTPUTS
	${FLEX_JsLexer_OUTPUTS}

	PARENT_SCOPE
)

add_executable(js main.cc ${SOURCES} ${HEADERS} ${FLEX_JsLexer_OUTPUTS})
target_include_directories(js PRIVATE . ..)

target_include_directories(libjs PRIVATE . ..)
target_sources(libjs PUBLIC ${SOURCES} ${HEADERS})

target_link_libraries(js fmt::fmt)
target_link_libraries(libjs fmt::fmt)

add_library(libjs::libjs ALIAS libjs)
