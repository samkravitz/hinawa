if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options("-DDEBUG_PRINT_CODE")
	add_compile_options("-DDEBUG_PRINT_STACK")
	add_compile_options("-DDEBUG_PRINT_AST")
endif()

set(SOURCES
	bindings/document_wrapper.cc
	bindings/node_wrapper.cc

	array.cc
	chunk.cc
	compiler.cc
	heap.cc
	object_string.cc
	object.cc
	parser.cc
	prelude.cc
	scanner.cc
	token.cc
	value.cc
	vm.cc

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

	bindings/document_wrapper.h
	bindings/node_wrapper.h
	bindings/wrapper.h

	array.h
	cell.h
	chunk.h
	compiler.h
	function.h
	heap.h
	object_string.h
	object.h
	opcode.h
	parser.h
	prelude.h
	primitive_string.h
	scanner.h
	token_type.h
	token.h
	value.h
	vm.h

	../document/document.cc
	../document/node.cc
	../document/element.cc
	../document/text.cc
)

flex_target(JsLexer2 lex.ll lex.yy.cc)

add_executable(js main.cc ${SOURCES} ${HEADERS} ${FLEX_JsLexer2_OUTPUTS})

target_include_directories(js PRIVATE . ..)
target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_link_libraries(js fmt::fmt)

target_include_directories(hinawa.test PRIVATE ..)
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
