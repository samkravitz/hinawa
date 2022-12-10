set(SOURCES
	chunk.cc
	compiler.cc
	function.cc
	parser.cc
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

	chunk.h
	compiler.h
	opcode.h
	parser.h
	scanner.h
	token_type.h
	token.h
	value.h
	vm.h
)

flex_target(JsLexer2 lex.ll lex.yy.cc)

add_executable(js main.cc ${SOURCES} ${HEADERS} ${FLEX_JsLexer2_OUTPUTS})
target_include_directories(js PUBLIC .)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
